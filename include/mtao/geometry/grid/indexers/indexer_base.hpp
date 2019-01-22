#pragma once
#include "mtao/geometry/grid/grid_utils.h"
#include <functional>
#include <numeric>
namespace mtao {
    namespace geometry {
        namespace grid {
            namespace indexing {
                namespace internal {
                    template <size_t D, typename T, size_t... N>
                        constexpr size_t size_from_shape_(std::integer_sequence<size_t,N...>,const std::array<T,D>& shape) {
                            return (shape[N] * ...);
                        }
                    template <size_t D, typename T>
                        constexpr size_t size_from_shape(const std::array<T,D>& shape) {
                            //return std::accumulate(shape.begin(),shape.end(),std::multiplies<T>());
                            return size_from_shape_(std::make_index_sequence<D>(), shape);
                        }
                }

                template <int D_, typename Derived>
                    class IndexerBase{
                        public:
                            static constexpr int D = D_;
                            using coord_type = std::array<int,D>;
                            Derived& derived() { return *static_cast<Derived*>(this); }
                            const Derived& derived() const { return *static_cast<const Derived*>(this); }

                            IndexerBase() = default;
                            IndexerBase(IndexerBase&&) = default;
                            IndexerBase(const IndexerBase&) = default;
                            IndexerBase& operator=(IndexerBase&&) = default;
                            IndexerBase& operator=(const IndexerBase&) = default;
                            IndexerBase(const coord_type& s): m_shape(s) {}
                            void resize(const coord_type& idx) {
                                m_shape = idx;
                            }
                            template <typename... Args>
                                size_t index(Args... args) const {
                                    static_assert(sizeof...(Args) == D);
                                    static_assert((std::is_convertible_v<Args,int> && ...));
                                    return derived().index(std::forward<Args>(args)...);
                                }

                            coord_type unindex(size_t ind) const {
                                return derived().unindex(ind);
                            }

                            template <typename... Args>
                            size_t operator()(Args&&... args) const {
                                return index(std::forward<Args>(args)...);
                            }

                            const coord_type& shape() const {return m_shape;}
                            int shape(size_t idx) const {return m_shape[idx];}
                            int width(size_t idx) const {return m_shape[idx];}
                            template <int Dim>
                                int width() const {return m_shape[Dim];}

                            int rows()const  {return width<0>();}
                            int cols()const  {return width<1>();}
                            size_t size() const {return internal::size_from_shape(m_shape);}

                        private:
                            coord_type m_shape = {};//zero-initializes
                    };

            }
        }
    }
}
