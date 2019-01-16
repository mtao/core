#pragma once
#include "grid_utils.h"
namespace mtao {
    namespace geometry {
        namespace grid {
            namespace indexing {
                template <int D_>
                    class OrderedIndexer{
                        public:
                            static constexpr int D = D_;
                            using index_type = std::array<int,D>;
                            OrderedIndexer() = default;
                            OrderedIndexer(OrderedIndexer&&) = default;
                            OrderedIndexer(const OrderedIndexer&) = default;
                            OrderedIndexer& operator=(OrderedIndexer&&) = default;
                            OrderedIndexer& operator=(const OrderedIndexer&) = default;
                            OrderedIndexer(const index_type& s): m_shape(s) {}
                            void resize(const index_type& idx) {
                                m_shape = idx;
                            }
                            template <int Dim=0, typename... Args>
                                size_t index(size_t a,Args... args) const {
                                    assert(int(a) < m_shape[Dim]);
                                    return a + m_shape[Dim] * index<Dim+1>(args...);
                                }
                            template <int Dim=D-1>
                                size_t index(size_t a) const {
                                    assert(int(a) < m_shape[Dim]);
                                    static_assert(Dim == D-1 || Dim == 0,"");
                                    return a;
                                }
                            size_t index(const index_type& a) const {
                                return utils::grid_index<D>(m_shape,a);
                            }
                            static constexpr size_t size(const index_type& shape) {
                                //return std::accumulate(shape.begin(),shape.end(),1,[](size_t a,size_t b) {return a*b;});
                                size_t res = 1;
                                for(auto&& p: shape) {
                                    res *= p;
                                }
                                return res;

                            }

                            const index_type& shape() const {return m_shape;}
                            int shape(size_t idx) const {return m_shape[idx];}
                            int width(size_t idx) const {return m_shape[idx];}
                            template <int Dim>
                                int width() const {return m_shape[Dim];}

                            int rows()const  {return width<0>();}
                            int cols()const  {return width<1>();}
                            size_t size() {
                                return size(m_shape);
                            }

                        private:
                            index_type m_shape = utils::internal::zero_array<index_type>();
                    };
            }
        }
    }
}
