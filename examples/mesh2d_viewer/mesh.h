#ifndef MESH_H
#define MESH_H
#include <Eigen/Dense>


using GLfloat = float;
using GLuint = unsigned int;

class Mesh {
    public:
        using MatrixXgf = Eigen::Matrix<GLfloat, Eigen::Dynamic,Eigen::Dynamic>;
        using MatrixXui = Eigen::Matrix<GLuint, Eigen::Dynamic,Eigen::Dynamic>;
        Mesh(const std::string& filename);
        MatrixXgf V;
        MatrixXui F;

    private:


};

#endif//MESH_H
