#pragma once
#include <algorithm>
#include "mtao/types.hpp"
#include "grid_utils.h"
#include "grid_storage.h"
#include "indexers/ordered_indexer.hpp"
#include <Eigen/Dense>


namespace mtao {
    namespace geometry {
        namespace grid {

            template <typename T,typename Indexer, typename Allocator = ::mtao::allocator<T>, typename StorageType = internal::grid_storage<T,Allocator>>
                class GridData: public Indexer {
                    public:
                        using storage_type = StorageType;
                        //using storage_type = internal::grid_storage<T,Allocator>;
                        using value_type = T;
                        using Scalar = T;//for eigen compat
                        static constexpr int D = Indexer::D;
                        using coord_type = typename Indexer::coord_type;
                        using Indexer::shape;
                        using Indexer::index;
                        using Indexer::valid_index;
                        using Indexer::width;

                        using iterator_type = typename storage_type::iterator_type;
                        using const_iterator_type = typename storage_type::const_iterator_type;
                        GridData(const coord_type& a): Indexer(a), m_storage(indexing::internal::size_from_shape(a)) {
                            init_data();
                        }
                        template <typename... Args>
                            GridData(Args... args): GridData(coord_type{{static_cast<int>(args)...}}) {
                                static_assert(sizeof...(args)== D);
                            }
                        GridData() {}
                        GridData(const GridData& other) = default;
                        //GridData(GridData&& other) = default;
                        GridData& operator=(const GridData& other) = default;
                        iterator_type begin() { return m_storage.begin(); }
                        iterator_type end() { return m_storage.end(); }
                        const_iterator_type begin() const { return m_storage.begin(); }
                        const_iterator_type end() const { return m_storage.end(); }
                        void resize(const coord_type& idx) {
                            Indexer::resize(idx);
                            size_t sfs = Indexer::size();
                            if(sfs != m_storage.size()) {
                                m_storage.resize(sfs);
                            }
                        }


                        template <typename... Args>
                            T& operator()(Args... args) {return m_storage(index(args...));}
                        template <typename... Args>
                            const T& operator()(Args... args) const {return m_storage(index(args...));}
                        T& get(size_t a) {return m_storage(a);}
                        const T& get(size_t a) const {return m_storage(a);}

                        T& operator()(const coord_type& t) {return m_storage(index(t));}
                        const T& operator()(const coord_type& t) const {return m_storage(index(t));}
                        T* data() {return m_storage.data();}
                        const T* data() const {return m_storage.data();}
                        auto as_eigen_vector() { return Eigen::Map<VectorX<T>>(data(),size()); }
                        auto as_eigen_vector() const { return Eigen::Map<const VectorX<T>>(data(),size()); }

                        size_t size() const {return m_storage.size();}
                        bool empty() const {return m_storage.empty();}

                        template <typename... Args>
                            void set_constant(Args... args) {
                                for(auto&& p: *this) {
                                    p = value_type(args...);
                                }
                            }
                        template <typename OpType>
                            GridData cwiseUnaryOp(const OpType& op = OpType()) const {
                                GridData g(shape());
                                std::transform(begin(),end(),g.begin(),op);
                                return g;
                            }
                        template <typename OpType>
                            GridData cwiseBinaryOp(const GridData& other, const OpType& op = OpType()) const {
                                assert(shape() == other.shape());
                                GridData g(shape());
                                std::transform(begin(),end(),other.begin(),g.begin(),op);
                                return g;
                            }
                        GridData operator+(const GridData& other) const {
                            GridData g(shape);
                            g.as_eigen_vector() = this->as_eigen_vector() + other.as_eigen_vector();
                            return g;
                            //return cwiseBinaryOp(other,[](auto&& a, auto&& b) {return a+b;});
                        }
                        GridData operator-(const GridData& other) const {
                            GridData g(shape());
                            g.as_eigen_vector() = this->as_eigen_vector() - other.as_eigen_vector();
                            return g;
                            //return cwiseBinaryOp(other,[](auto&& a, auto&& b) {return a-b;});
                        }

                        template <typename U>
                            GridData operator*(const U& v) const {
                                GridData g(shape());
                                g.as_eigen_vector() = v * this->as_eigen_vector();
                                return g;
                                //return cwiseUnaryOp([&v](auto&& a) {return v*a;});
                            }
                        template <typename U>
                            GridData operator/(const U& v) const {
                                GridData g(shape());
                                g.as_eigen_vector() = this->as_eigen_vector().array() / v;
                                return g;
                                //return cwiseUnaryOp([&v](auto&& a) {return a/v;});
                            }
                            GridData operator!() const {
                                return cwiseUnaryOp([](auto&& b) { return !b; });
                            }
                        ////read only just over indices
                        //void loop(const std::function<void(const coord_type&)>& f) const {
                        //    utils::multi_loop(m_shape,[&](auto&& v) {
                        //            f(v);
                        //            });
                        //}
                        //read only with values
                        void loop(const std::function<void(const coord_type&, const Scalar&)>& f) const {
                            utils::multi_loop(shape(),[&](auto&& v) {
                                    f(v,(*this)(v));
                                    });
                        }
                        void loop_parallel(const std::function<void(const coord_type&, const Scalar&)>& f) const {
                            utils::multi_loop_parallel(shape(),[&](auto&& v) {
                                    f(v,(*this)(v));
                                    });
                        }
                        //write using index and current value
                        void loop_write(const std::function<Scalar(const coord_type&, const Scalar&)>& f) {
                            utils::multi_loop(shape(),[&](auto&& v) {
                                    auto&& p = (*this)(v);
                                    p = f(v,p);
                                    });
                        }
                        void loop_write_parallel(const std::function<Scalar(const coord_type&, const Scalar&)>& f) {
                            utils::multi_loop_parallel(shape(),[&](auto&& v) {
                                    auto&& p = (*this)(v);
                                    p = f(v,p);
                                    });
                        }
                        //write using only index
                        void loop_write_idx(const std::function<Scalar(const coord_type&)>& f) {
                            utils::multi_loop(shape(),[&](auto&& v) {
                                    (*this)(v) = f(v);
                                    });
                        }
                        void loop_write_idx_parallel(const std::function<Scalar(const coord_type&)>& f) {
                            utils::multi_loop_parallel(shape(),[&](auto&& v) {
                                    (*this)(v) = f(v);
                                    });
                        }
                        //void cwiseUnaryOp(const std::function<Scalar(const Scalar&)>& f) {
                        //    std::transform(begin(),end(),begin(),f);
                        //}

                    public:
                        template <typename... Args>
                            static GridData Constant(const value_type& v,Args&&... args ) {
                                GridData g(std::forward<Args>(args)...);
                                g.set_constant(v);
                                return g;
                            }

                    private:
                        void init_data() {
                            set_constant(T{});
                        }
                        storage_type m_storage;


                };
            template <typename T, int D, typename Allocator = ::mtao::allocator<T>, typename StorageType = internal::grid_storage<T,Allocator>>
                using GridDataD = GridData<T,indexing::OrderedIndexer<D>,Allocator,StorageType>;
            using GridData2f = GridDataD<float,2>;
            using GridData3f = GridDataD<float,3>;
            using GridData2i = GridDataD<int,2>;
            using GridData3i = GridDataD<int,3>;
            using GridData2d = GridDataD<double,2>;
            using GridData3d = GridDataD<double,3>;
            /*
               template <typename T, int D, typename Allocator = mtao::allocator<T>>
               using DynamicGrid = Grid<T,Allocator,internal::grid_storage<T,Allocator>>;
               template <typename T, int D, typename Allocator = mtao::allocator<T>>
               using StaticGrid = DynamicGrid<T,D,Allocator,internal::static_storage<T>>;
               template <typename T, int D, typename Allocator = mtao::allocator<T>>
               using Grid = DynamicGrid<T,D,Allocator>;
               */


        }

    }
}

