#ifndef TRIANGLE_MESH_H
#define TRIANGLE_MESH_H

#include <Eigen/Dense>
#include <vector>


class TriangleMesh {
    public:
        using Mat3Xf = Eigen::Matrix<float,3, Eigen::Dynamic>;
        using Mat3Xi = Eigen::Matrix<int,3, Eigen::Dynamic>;





    private:
        Mat3Xf m_vertices;
        Mat3i  m_triangles;
        


};



#endif//TRIANGLE_MESH_H

