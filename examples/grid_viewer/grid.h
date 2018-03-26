#pragma once
#include <mtao/types.h>
#include <glad/glad.h>


inline int index(const int i, const int j, int ii, int jj) {
    return ii + i * jj;
}
mtao::ColVectors<GLuint,3> make_topology(const int i, const int j);
mtao::ColVectors<GLfloat,2> make_vertices(const int i, const int j);
std::tuple<mtao::ColVectors<GLfloat,2>,mtao::ColVectors<GLuint,3>> make_mesh(const int i, const int j);

