#include "mesh.h"
#include <fstream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <array>
#include <iostream>



using Vec = std::array<GLfloat,3>;
using Tri = std::array<GLuint,3>;
using TokIt = std::vector<std::string>::const_iterator;



static Vec process_vertex(TokIt begin, const TokIt& end) {
    Vec v;
    std::transform(begin,begin+3,v.begin(),[](const std::string& s) {
            return std::stof(s);});

    size_t dist = std::distance(begin,end);
    if(dist == 4) {
        float w = std::stof(*(begin+3));
        for(auto&& s: v) {
            s/= w;
        }
    }
    return v;
}


template <int N>
int get_slash_token(const std::string& str) {
    std::string tok;
    std::stringstream ss(str);
    for(int i = 0; i <= N; ++i) {
        std::getline(ss,tok,'/');
    }
    return std::stod(tok);
}


Mesh::Mesh(const MatrixXgf& V, const MatrixXui& F): V(V), F(F) {
}

Mesh::Mesh(const std::string& filename) {
    std::vector<Vec> vecs;
    std::vector<Tri> tris;

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
            if(front[0] == 'f') {
                GLuint first = get_slash_token<0>(tokens[1]);
                GLuint left = get_slash_token<0>(tokens[2]);
                for(size_t i = 3; i < tokens.size(); ++i) {
                    GLuint right = get_slash_token<0>(tokens[i]);
                    tris.emplace_back(Tri{{first,left,right}});
                    left = right;
                }
            }
        }
    }

    F.resize(3,tris.size());
    V.resize(3,vecs.size());

    using Vec3f = Eigen::Matrix<GLfloat,3,1>;
    using Mat3f = Eigen::Matrix<GLfloat,3,3>;

    for(int i = 0; i < V.cols(); ++i) {
        V.col(i) = Eigen::Map<Vec3f>(&vecs[i].front());
    }

    for(int i = 0; i < F.cols(); ++i) {
        F.col(i) = Eigen::Map<Eigen::Array<GLuint,3,1>>(&tris[i].front()) - 1; //OBJ uses 1 indexing!
    }


    Vec3f center;
    GLfloat vol;
    // \|x - p_i\| = \|x\|
    // <x,x> - 2<x,p_i> + <p_i,p_i>^2 - <x,x>
    // 2<x,p_i> + <p_i,p_i>^2 
    V.colwise() -= (V.rowwise().sum() / V.cols()).eval();
    return;
    for(int i = 0; i < F.cols(); ++i) {
        Mat3f A;
        Vec3f r;
        for(int j = 0; j < 3; ++j) {
            auto&& v = V.col(F(j,i));
            A.row(j) = 2*v.transpose();
            r(j) = v.squaredNorm();
        }
        //Vec3f x = A.jacobiSvd(Eigen::ComputeFullU | Eigen::ComputeFullV).solve(r);
        Vec3f x = A.colPivHouseholderQr().solve(r);
        GLfloat myvol = A.determinant()/6.0;

        vol += myvol;
        center +=  myvol * x;
    }
        std::cout << "=======" << std::endl;
    center.array() /= vol;
    std::cout << "Center: " << center.transpose() << std::endl;
    std::cout << "Volume: " << vol << std::endl;
    V.colwise() -= center;
}

