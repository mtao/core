#pragma once
#include "mtao/geometry/mesh/plc.hpp"
#include "mtao/types.h"
#include <fstream>

namespace mtao { namespace geometry { namespace mesh {

    template <typename T, int Dim, int Dim2>
        void write_file(const std::string& filename, const mtao::ColVectors<T,Dim>& D, const mtao::ColVectors<T,Dim2>& A, const MarkerType& M) {

            constexpr static bool TriangleMode = Dim == 2;
            constexpr static bool TetGenMode = Dim == 3;
            std::ofstream ofs(filename);

            bool attributes_enabled = (size == A.rows()) && Dim2 != 0;
            bool markers_enabled = size == M.rows();

            int size = D.cols();
            ofs << size;
            if constexpr(std::is_same_v<T,int>) {
                if(Dim == 2) {
                    ofs << " " << 0 << std::endl;
                }
            } else {
                ofs << " " << D.rows() << << " " << A.rows();
                if(markers_enabled) {
                    ofs << " " << 1 << std::endl;
                } else {
                    ofs << std::endl;
                }
            }

            for(int i = 0; i < size; ++i) {
                if constexpr(std::is_same_v<T,int>) {
                    ofs << D.col(i).transpose();
                } else {//outputting a node file
                    ofs << i << " " << D.col(i).transpose();
                }
                if(attributes_enabled) {
                    ofs << " " << A.col(i).transpose();
                }
                if(markers_enabled) {
                    ofs << " " << M(i) << std::endl;
                } else {
                    ofs << std::endl;
                }
            }



        }

    template <typename T, int D, typename T2 = int, int D2 = 0>
    void write_node(const std::string& filenameconst mtao::ColVectors<T,D>&A , const mtao::ColVectors<T2,D2>& B= mtao::ColVectors<T2,D2>(), const plc::MarkerType& M= plc::MarkerType()){write_file(filename,A,B,M);}

    template <typename T=int, int D2=0>
    void write_edge(const std::string& filename, const plc::IndexType<2>&A, const mtao::ColVectors<T,D2>& B= mtao::ColVectors<T,D2>(),const plc::MarkerType& M= plc::MarkerType()) {write_file(filename,A,B,M);}
    template <typename T=int, int D2=0>
    void write_face(const std::string& filename, const plc::IndexType<3>&A, const mtao::ColVectors<T,D2>& B= mtao::ColVectors<T,D2>(),const plc::MarkerType& M= plc::MarkerType()){write_file(filename,A,B,M);}
    template <typename T=int, int D2=0>
    void write_ele(const std::string& filename,  const plc::IndexType<4>&A, const mtao::ColVectors<T,D2>& B= mtao::ColVectors<T,D2>(),const plc::MarkerType& M= plc::MarkerType()){write_file(filename,A,B,M);}

}}}

