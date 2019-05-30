#include "geometry/mesh/constructor.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iterator>
#include <vector>
#include <cassert>
#include <algorithm>
#include <cstdlib>

Mesh Mesh::fromOBJ(const std::string& filename) {
    Mesh mesh;

    std::ifstream ifs(filename.c_str());

    for(std::string line; std::getline(ifs,line);) {
        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::copy(std::istream_iterator<std::string>(iss)
                , std::istream_iterator<std::string>()
                , std::back_insert_iterator<std::vector<std::string> >(tokens));
        mesh.parseLine(tokens);
    }
    return mesh;
}

namespace internal {
    struct lparser {//parse line for a/b/c/d to get a
        int operator()(const std::string& s) {
            std::istringstream ss(s);
            std::string vs;
            std::getline(ss,vs,'/');
            return std::stoi(vs);
        }
    };
}
void Mesh::parseVertex(const std::vector<std::string>& line) {
    assert(line.size() == 4);
    std::array<Scalar,3> p;
    std::transform(line.begin()+1,line.end(), &p[0], std::stod);
    m_mesh.add_vertex(p[0],p[1],p[2]);
}
void Mesh::parseFace(const std::vector<std::string>& line) {
    auto lparser = [](const std::string& s) {
            std::istringstream ss(s);
            std::string vs;
            std::getline(ss,vs,'/');
            return std::stoi(vs);
        };
    assert(line.size() == 4 || line.size() == 5);//allow quads
    std::array<size_t,3> i;
    std::transform(line.begin()+1,line.end(), &i[0], internal::lparser());
    m_mesh.add_triangle(i[0],i[1],i[2]);
    if(line.size() == 5) {//only modify the last two indices for the new triangle
    std::transform(line.begin()+3,line.end(), &i[0], internal::lparser());
    m_mesh.add_triangle(i[0],i[1],i[2]);
    }
}

void Mesh::parseLine(const std::vector<std::string>& line) {
    const std::string& type = line[0];
    if(type == "v") {
        parseVertex(line);
    } else if(type == "f") {
        parseFace(line);
    } else {
        //Assume it's a comment
    }
}

int main(int argc, char * argv[]) {
    if(argc < 2) {
        std::cout << "Nothing here!" << std::endl;
        return 1;
    }
    Mesh m = Mesh::fromOBJ(argv[1]);
    std::cout << m.getPoints().size() << " " << m.getIndices().size() << std::endl;
    return 0;
}
