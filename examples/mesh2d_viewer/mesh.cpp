#include "mesh.h"
#include <fstream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <array>
#include <iostream>



using Vec = std::array<GLfloat,2>;
using Tri = std::array<GLuint,3>;
using TokIt = std::vector<std::string>::const_iterator;



static Vec process_vertex(TokIt begin, const TokIt& end) {
    Vec v;
    std::transform(begin,begin+2,v.begin(),[](const std::string& s) {
            return std::stof(s);});

    /*
    size_t dist = std::distance(begin,end);
    if(dist == 4) {
        float w = std::stof(*(begin+3));
        for(auto&& s: v) {
            s/= w;
        }
    }
    */
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


static Tri process_triangle (TokIt begin, const TokIt& end) {
    Tri t;

    std::transform(begin,end,t.begin(),[](const std::string& s) {
            return get_slash_token<0>(s);
            });
    return t;
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
                tris.emplace_back(process_triangle(tokens.begin()+1,tokens.end()));
                if(tokens.size() == 5) {
                    GLuint last = get_slash_token<0>(tokens.back());
                    auto&& p = tris.back();
                    tris.emplace_back(Tri{{p[0],p[2],last}});
                }
            }
        }
    }

    F.resize(3,tris.size());
    V.resize(2,vecs.size());

    using Vec2f = Eigen::Matrix<GLfloat,2,1>;
    using Mat2f = Eigen::Matrix<GLfloat,2,2>;


    for(int i = 0; i < V.cols(); ++i) {
        V.col(i) = Eigen::Map<Vec2f>(&vecs[i].front());
    }


    for(int i = 0; i < F.cols(); ++i) {
        F.col(i) = Eigen::Map<Eigen::Array<GLuint,3,1>>(&tris[i].front()) - 1; //OBJ uses 1 indexing!
        F.col(i) = Eigen::Map<Eigen::Array<GLuint,3,1>>(&tris[i].front()) ; //OBJ uses 1 indexing!
    }


    V.colwise() -= (V.rowwise().sum() / V.cols()).eval();
    Eigen::AlignedBox<float,2> bb;
    for(int i = 0; i < V.cols(); ++i) {
        bb.extend(V.col(i));
    }
}

