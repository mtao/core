#include <array>
#include "mtao/geometry/mesh/read_ply.hpp"
#include <fstream>
#include <iterator>
#include "mtao/logging/logger.hpp"

using namespace mtao::logging;

namespace mtao { namespace geometry { namespace mesh {


    namespace ply_internal {
        struct TokenPopper {
            public:


                TokenPopper(const std::ifstream& ifs): ifs(ifs) {}
                TokenPopper(const TokenPopper&) = default;
                TokenPopper(TokenPopper&&) = default;
                const std::optional<std::vector<std::string>>& increment() {
                    current.emplace();
                    tokens = *current;
                    std::string line;
                    if(!std::getline(ifs,line)) {
                        return {};
                    }
                    std::istringstream iss(line);
                    std::copy(std::istream_iterator<std::string>(iss)
                            , std::istream_iterator<std::string>()
                            , std::back_insert_iterator<std::vector<std::string> >(tokens));
                    return current;
                }

                operator bool() const {
                    return ifs;
                }

                const std::optional<std::vector<std::string>>& tokens() const { return current; }

            private:
                std::optional<std::vector<std::string>> current;
                std::ifstream& ifs;
        };
        std::optional<std::vector<std::string>> pop_line_tokens(std::ifstream& ifs) {
            if(ifs) {
                return TokenPopper(ifs).increment();
            } else {
                return {};
            }
        }


        template <typename T>
            struct VecX_wrapper {};
        template <typename... Args> 
            struct VecX_wrapper<std::variant<Args...>> {
                using value = std::variant<mtao::VectorX<Args>...>;
                
            };

    struct ElementType {
        using elem_type = std::variant<char,uchar,short,unsigned short,int,unsigned int,float,double>;
        using vec_type = std::variant



        using property_list = std::vector<std::tuple<std::string, std::size_t>>;
        ElementType::ElementType(int size, TokenPopper& tp): size(size) {
            for(; tp; tp.increment()) {

                auto&& tokopt = tokens;

                if(!tokopt) {
                    std::cout << "WTF bak tokeopt?" << std::endl;
                }
                auto& tokens = *tokopt;
                if(tokens.empty()) {
                    continue;
                } else {
                    auto&& front = tokens.front();
                    if(front == "property") {

                        const auto& type = tokens[1];
                        int index = 0;
                        if(type == "char") {
                            index = elem_type::index<char>();
                        } else if(type == "uchar") {
                            index = elem_type::index<unsigned char>();
                        } else if(type == "short") {
                            index = elem_type::index<short>();
                        } else if(type == "ushort") {
                            index = elem_type::index<unsigned short>();
                        } else if(type == "int") {
                            index = elem_type::index<int>();
                        } else if(type == "uint") {
                            index = elem_type::index<unsigned int>();
                        } else if(type == "float") {
                            index = elem_type::index<float>();
                        } else if(type == "double") {
                            index = elem_type::index<double>();
                        }
                        properties.emplace_back(tokens[2],index);
                    } else {
                        return;
                    }
                   
                }
            }
        }
        template <typename T,int D>
             process(TokenPopper& tp) {
                mtao::ColVectors<T,D> R(D,size);
                for(int i = 0; i < 
            }

        private:
        proerty_list properties;
        size_t size;

    };
    }

    template <typename T>
        std::tuple<mtao::ColVectors<T,3>,mtao::ColVectors<int,3>> read_ply(const std::string& filename) {
            std::ifstream ifs(filename);
            ply_internal::TokenPopper tokpop(ifs);
            std::string line;
            std::getline(ifs,line);
            assert(typestr == "ply");
            std::getline(ifs,line);
            assert(typestr == "format ascii 1.0");


            ElementType vertices;
            ElementType edges;
            ElementType faces;







            //Header

            for(; tokopt;tokopt = pop_line_tokens(ifs)) {
                if(!tokopt) {
                    std::cout << "WTF bak tokeopt?" << std::endl;
                }
                auto& tokens = *tokopt;


                if(tokens.empty()) {
                    continue;
                } else {
                    auto&& front = tokens.front();
                    if(front == "comment") {
                        continue;
                    } else if(front == "element") {
                        const auto& elem_type = tokens[1];
                        if(elem_type == "vertex") {
                            num_vertices = std::atoi(tokens[2]);
                        } else if(elem_type == "face") {
                            num_faces = std::atoi(tokens[2]);
                        } else if(elem_type == "edge") {
                            num_edges = std::atoi(tokens[2]);
                        }
                    }
                }
            }
            //Vertex 

        }
    template <typename T>
        struct MeshReader {

            using Vec = std::array<T,3>;
            using Tri = std::array<int,3>;
            using TokIt = std::vector<std::string>::const_iterator;



            static Vec process_vertex(TokIt begin, const TokIt& end) {
                Vec v = {{0,0}};
                std::transform(begin,end,v.begin(),[](const std::string& s) {
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
                static int get_slash_token(const std::string& str) {
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
            static std::tuple<mtao::ColVectors<T,3>,mtao::ColVectors<int,3>> read_ply(const std::string& filename) {
                std::vector<Vec> vecs;
                std::vector<Tri> tris;

                auto add_valid_triangle = [&](const Tri& t) {
                    if(
                            (t[0] != t[1])
                            && (t[0] != t[2])
                            && (t[1] != t[2])
                      ) {
                        tris.emplace_back(t);
                    }
                };

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
                            add_valid_triangle(process_triangle(tokens.begin()+1,tokens.end()));
                            if(tokens.size() == 5) {
                                int last = get_slash_token<0>(tokens.back());
                                auto&& p = tris.back();
                                add_valid_triangle(Tri{{p[0],p[2],last}});
                            }
                        }
                    }
                }

                mtao::ColVectors<T,3> V(3,vecs.size());
                mtao::ColVectors<int,3> F(3,tris.size());

                using Vec3 = Eigen::Matrix<T,3,1>;

                for(int i = 0; i < V.cols(); ++i) {
                    V.col(i) = Eigen::Map<Vec3>(&vecs[i].front());
                }

                for(int i = 0; i < F.cols(); ++i) {
                    F.col(i) = Eigen::Map<Eigen::Array<int,3,1>>(&tris[i].front()) - 1; //OBJ uses 1 indexing!
                }
                return {V,F};
            }
        };

    std::tuple<mtao::ColVectors<double,3>,mtao::ColVectors<int,3>> read_plyD(const std::string& filename) {
        return MeshReader<double>::read_ply(filename);
    }
    std::tuple<mtao::ColVectors<float,3>,mtao::ColVectors<int,3>> read_plyF(const std::string& filename) {
        return MeshReader<float>::read_ply(filename);
    }

}}}
