#include "mtao/geometry/mesh/read_ply.hpp"
#include "mtao/eigen/stack.h"
#include <fstream>


namespace {
    template <typename T>
        std::string ply_type_str() {
            if(std::is_same_v<T,char>) {
                return "char";
            } else if(std::is_same_v<T,unsigned char>) {
                return "uchar";
            } else if(std::is_same_v<T,short>) {
                return "short";
            } else if(std::is_same_v<T,unsigned short>) {
                return "ushort";
            } else if(std::is_same_v<T,int>) {
                return "int";
            } else if(std::is_same_v<T,unsigned int>) {
                return "uint";
            } else if(std::is_same_v<T,float>) {
                return "float";
            } else if(std::is_same_v<T,double>) {
                return "double";
            }
        }
}
namespace mtao::geometry::mesh {
    template <typename T>
        void write_ply(const mtao::ColVectors<T,3>& V, const mtao::ColVectors<T,3>& C,const mtao::ColVectors<int,3>& F, const std::string& filename) {
            std::string tstr = ply_type_str<T>();
            std::string istr = ply_type_str<int>();
            std::ofstream ofs(filename.c_str());
            ofs << "ply\n";
            ofs << "format ascii 1.0\n";
            ofs << "element vertex " << V.cols() << std::endl;
            ofs << "property " << tstr << " x" << std::endl;;
            ofs << "property " << tstr << " y" << std::endl;;
            ofs << "property " << tstr << " z" << std::endl;;
            if(C.cols() == V.cols()) {
                //ofs << "property " << tstr << " red" << std::endl;;
                //ofs << "property " << tstr << " green" << std::endl;;
                //ofs << "property " << tstr << " blue" << std::endl;;
                ofs << "property uchar red" << std::endl;;
                ofs << "property uchar green" << std::endl;;
                ofs << "property uchar blue" << std::endl;;
            }
            ofs << "element face " << F.cols() << std::endl;
            ofs << "property list uchar int vertex_indices" << std::endl;
            ofs << "end_header" << std::endl;
            if(C.cols() == V.cols()) {
            mtao::ColVectors<T,3> C2 = (255*C.array()).round().cwiseMin(255).cwiseMax(0);
            for(int i = 0; i < V.cols(); ++i) {
                ofs << V.col(i).transpose() << " ";
                auto c = C2.col(i);
                ofs << c.transpose() << std::endl;
                //for(int j = 0; j < 3; ++j) {
                //    ofs << c(j) << " ";
                //}
                //ofs <<std::endl;
            }
                //ofs << mtao::eigen::vstack(V,C2).transpose() << std::endl;
            } else {
                ofs << V.transpose() << std::endl;
            }


            ofs << mtao::eigen::vstack(mtao::RowVecXi::Constant(F.cols(),3),F).transpose() << std::endl;
        }

    void write_plyF(const mtao::ColVectors<float,3>& V,const mtao::ColVectors<int,3>& F, const std::string& filename) {
        write_ply(V,{},F,filename);
    }
    void write_plyD(const mtao::ColVectors<double,3>& V,const mtao::ColVectors<int,3>& F, const std::string& filename) {
        write_ply(V,{},F,filename);
    }
    void write_plyF(const mtao::ColVectors<float,3>& V, const mtao::ColVectors<float,3>& C, const mtao::ColVectors<int,3>& F, const std::string& filename) {
        write_ply(V,C,F,filename);
    }
    void write_plyD(const mtao::ColVectors<double,3>& V, const mtao::ColVectors<double,3>& C,const mtao::ColVectors<int,3>& F, const std::string& filename) {
        write_ply(V,C,F,filename);
    }
}
