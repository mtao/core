#include "mtao/geometry/mesh/read_obj.hpp"
#include <fstream>


namespace mtao::geometry::mesh {
template<typename T>
void write_obj(const mtao::ColVectors<T, 3> &V, const mtao::ColVectors<int, 3> &F, const std::string &filename) {
    std::ofstream ofs(filename.c_str());
    for (int i = 0; i < V.cols(); ++i) {
        ofs << "v " << V.col(i).transpose() << std::endl;
    }
    for (int i = 0; i < F.cols(); ++i) {
        ofs << "f " << (F.col(i).array() + 1).transpose() << std::endl;
    }
}

/*
    template <>
    void write_obj(const mtao::ColVectors<float,3>& V,const mtao::ColVectors<int,3>& F, const std::string& filename);

    template <>
    void write_obj(const mtao::ColVectors<double,3>& V,const mtao::ColVectors<int,3>& F, const std::string& filename);

    */
void write_objF(const mtao::ColVectors<float, 3> &V, const mtao::ColVectors<int, 3> &F, const std::string &filename) {
    write_obj(V, F, filename);
}
void write_objD(const mtao::ColVectors<double, 3> &V, const mtao::ColVectors<int, 3> &F, const std::string &filename) {
    write_obj(V, F, filename);
}
}// namespace mtao::geometry::mesh
