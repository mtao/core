#include "geometry.h"
#include <mtao/geometry/mesh/sphere.hpp>
#include <mtao/logging/timer.hpp>
#include <fstream>
#include <iterator>
using namespace mtao::logging;
using namespace mtao::geometry::mesh::triangle;
Shape::~Shape() {}
//Square::~Square() {}
//Transform::~Transform() {}
//Translate::~Translate() {}
//Scale::~Scale() {}

void Shape::set_vert_indices(const std::map<int,int>& map) {
    vert_marker_map = map;
}
int Shape::get_vert_index(int i) const {
    if(auto it = vert_marker_map.find(i); it != vert_marker_map.end()) {
        return it->second;
    } else {
        return i;
    }
}
void Shape::set_edge_indices(const std::map<int,int>& map) {
    edge_marker_map = map;
}
void Shape::apply_edge_markers(mtao::VectorX<int>& V) const {
    V.noalias() = V.unaryExpr([&](int i) {return get_edge_index(i); });
}
void Shape::apply_vert_markers(mtao::VectorX<int>& V) const {
    V.noalias() = V.unaryExpr([&](int i) {return get_vert_index(i); });
}
int Shape::get_edge_index(int i) const {
    if(auto it = edge_marker_map.find(i); it != edge_marker_map.end()) {
        return it->second;
    } else {
        return i;
    }
}
Transform::Transform(const std::shared_ptr<Shape>& s): shape(s) {}
        int Transform::get_vert_index(int i) const {
            return shape->get_vert_index(Shape::get_vert_index(i));
        }
        int Transform::get_edge_index(int i) const {
            return shape->get_edge_index(Shape::get_edge_index(i));
        }

Translate::Translate(const std::shared_ptr<Shape>& sh, REAL sx, REAL sy): Transform(sh), t(sx,sy) {}
Mesh Translate::boundary_geometry() {
    Mesh m = shape->boundary_geometry();
    m.translate(t);
    return m;
}

Scale::Scale(const std::shared_ptr<Shape>& sh, REAL sx, REAL sy): Transform(sh), s(sx,sy) {}
Scale::Scale(const std::shared_ptr<Shape>& sh, REAL s): Transform(sh), s(s,s) {}
Mesh Scale::boundary_geometry() {
    Mesh m = shape->boundary_geometry();
    m.scale(s);
    return m;
}

Square::Square(REAL r, int nx, int ny): radius(r), nx(nx), ny(ny) {}

Sphere::Sphere(REAL r, int vertcount): radius(r), vertcount(vertcount) {}

Mesh Sphere::boundary_geometry() {
    Mesh m = mtao::geometry::mesh::sphere<REAL,2>(vertcount);
    m.V *= radius;
    m.VA.setConstant(1);
    m.EA.setConstant(1);
    apply_vert_markers(m.VA);
    apply_edge_markers(m.EA);
    return m;
}

Mesh Square::boundary_geometry() {
    using Vec2 = mtao::Vector<REAL,2>;
    Mesh m;
    auto&& V = m.V;
    auto&& E = m.E;
    auto&& VA = m.VA;
    auto&& EA = m.EA;


    int size = 2 * (nx + ny);
    int stride = nx + ny;

    V.resize(2,size);
    E.resize(2,size);
    m.fill_attributes();

    REAL dx = 1.0 / nx;
    REAL dy = 1.0 / ny;
    VA.setConstant(get_vert_index(0));
    EA.setConstant(get_edge_index(0));

    for(int i = 0; i < nx; ++i) {
        V.col(i).template topRows<2>() = Vec2(i * dx,0);
        V.col(stride + nx - i - 1).template topRows<2>() = Vec2((i+1) * dx,1);
        EA(i) = get_edge_index(1);
        EA(stride + nx - i - 1) = get_edge_index(3);
        VA(i) = get_vert_index(1);
        VA(stride + nx - i - 1) = get_vert_index(1);
    }
    for(int i = 0; i < ny; ++i) {
        V.col(nx + i).template topRows<2>() = Vec2(1,i * dy);
        V.col(V.cols() - i - 1).template topRows<2>() = Vec2(0,(i+1) * dy);
        EA(nx + i) = get_edge_index(2);
        EA(V.cols() - i - 1) = get_edge_index(4);
        VA(nx + i) = get_vert_index(1);
        VA(V.cols() - i - 1) = get_vert_index(1);
    }
    for(int i = 0; i < E.cols(); ++i) {
        auto e = E.col(i);
        e(0) = i;
        e(1) = (i+1) % E.cols();
    }

    //V.array() -= .5;
    //V = 4 * V;
    V.array() *= 2;
    V.array() -= 1;
    V.array() *= radius;

    return m;
}

template <typename T>
struct EdgeMeshReader {

    using Vec = std::array<T,2>;
    using Edge = std::array<int,2>;
    using TokIt = std::vector<std::string>::const_iterator;



    static Vec process_vertex(TokIt begin, const TokIt& end) {
        Vec v = {{0,0}};
        std::transform(begin,end,v.begin(),[](const std::string& s) {
                return std::stof(s);});
        return v;
    }


        static Edge process_edge(TokIt begin, const TokIt& end) {
            Edge v = {{0,0}};
            std::transform(begin,end,v.begin(),[](const std::string& s) {
                    return std::stof(s);});
            return v;
        }


    static std::tuple<mtao::ColVectors<T,2>,mtao::ColVectors<int,2>,mtao::ColVectors<T,2>> read_obj(const std::string& filename) {
        std::vector<Vec> vecs;
        std::vector<Vec> holes;
        std::vector<Edge> edges;

        std::ifstream ifs(filename);

        for(std::string line; std::getline(ifs,line);) {
            std::istringstream iss(line);
            std::vector<std::string> tokens;
            std::copy(std::istream_iterator<std::string>(iss)
                    , std::istream_iterator<std::string>()
                    , std::back_insert_iterator<std::vector<std::string> >(tokens));


            if(tokens.empty()) {
                continue;
            } else {
                auto&& front = tokens.front();
                if(front[0] == 'v') {
                    if(front.size() == 1) {//cover for texture coordinates
                        vecs.emplace_back(process_vertex(tokens.begin()+1,tokens.end()));
                    }
                }
                if(front[0] == 'h') {
                    if(front.size() == 1) {//cover for texture coordinates
                        holes.emplace_back(process_vertex(tokens.begin()+1,tokens.end()));
                    }
                }
                if(front[0] == 'e') {
                    edges.emplace_back(process_edge(tokens.begin()+1,tokens.end()));
                }
            }
        }

        mtao::ColVectors<T,2> V(2,vecs.size());
        mtao::ColVectors<T,2> H(2,holes.size());
        mtao::ColVectors<int,2> F(2,edges.size());

        using Vec2 = Eigen::Matrix<T,2,1>;

        for(int i = 0; i < V.cols(); ++i) {
            V.col(i) = Eigen::Map<Vec2>(&vecs[i].front());
        }
        for(int i = 0; i < H.cols(); ++i) {
            H.col(i) = Eigen::Map<Vec2>(&holes[i].front());
        }

        for(int i = 0; i < F.cols(); ++i) {
            F.col(i) = Eigen::Map<Eigen::Array<int,2,1>>(&edges[i].front()) - 1; //OBJ uses 1 indexing!
        }
        return {V,F,H};
    }
};

std::tuple<mtao::ColVectors<double,2>,mtao::ColVectors<int,2>,mtao::ColVectors<double,2>> read_objD(const std::string& filename) {
    return EdgeMeshReader<double>::read_obj(filename);
}
Freehand::Freehand(const std::string& filename) {
    std::cout << "Reading file: " << filename << std::endl;
    std::tie(V,E,H) = read_objD(filename);

}
Freehand::Freehand(const mtao::ColVecs2d& V, const mtao::ColVecs2i& E, const mtao::ColVecs2d& H): V(V),E(E),H(H) {}
Mesh Freehand::boundary_geometry() {
    Mesh m(V,E);
    m.H = H;
    m.VA.setConstant(1);
    m.EA.setConstant(1);
    apply_vert_markers(m.VA);
    apply_edge_markers(m.EA);
    return m;
}
