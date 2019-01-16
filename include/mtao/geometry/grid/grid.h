#pragma once
#include <algorithm>
#include "grid_utils.h"
#include "grid_storage.h"


namespace mtao {
    namespace geometry {
        namespace grid {

            template <typename T, int D_, typename Allocator = ::mtao::allocator<T>, typename StorageType = internal::grid_storage<T,Allocator>>
                class Grid: public utils::GridIndexer<D_> {
                    public:
                        using storage_type = StorageType;
                        //using storage_type = internal::grid_storage<T,Allocator>;
                        using value_type = T;
                        using Scalar = T;//for eigen compat
                        static constexpr int D = D_;
                        using Indexer = utils::GridIndexer<D>;
                        using index_type = typename Indexer::index_type;
                        using Indexer::shape;
                        using Indexer::index;
                        using Indexer::width;

                        using iterator_type = typename storage_type::iterator_type;
                        using const_iterator_type = typename storage_type::const_iterator_type;
                        template <typename... Args>
                            Grid(Args... args): Grid(index_type{{static_cast<int>(args)...}}) {
                                static_assert(sizeof...(args)== D);
                            }
                        Grid(const index_type& a): Indexer(a), m_storage(Indexer::size(a)) {
                            init_data();
                        }
                        Grid() {}
                        Grid(const Grid& other) = default;
                        //Grid(Grid&& other) = default;
                        Grid& operator=(const Grid& other) = default;
                        iterator_type begin() { return m_storage.begin(); }
                        iterator_type end() { return m_storage.end(); }
                        const_iterator_type begin() const { return m_storage.begin(); }
                        const_iterator_type end() const { return m_storage.end(); }
                        void resize(const index_type& idx) {
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

                        T& operator()(const index_type& t) {return m_storage(index(t));}
                        const T& operator()(const index_type& t) const {return m_storage(index(t));}
                        T* data() {return m_storage.data();}
                        const T* data() const {return m_storage.data();}

                        size_t size() const {return m_storage.size();}

                        template <typename... Args>
                            void set_constant(Args... args) {
                                for(auto&& p: *this) {
                                    p = value_type(args...);
                                }
                            }
                        template <typename OpType>
                            Grid cwiseUnaryOp(const OpType& op = OpType()) const {
                                Grid g(shape());
                                std::transform(begin(),end(),g.begin(),op);
                                return g;
                            }
                        template <typename OpType>
                            Grid cwiseBinaryOp(const Grid& other, const OpType& op = OpType()) const {
                                assert(shape() == other.shape());
                                Grid g(shape());
                                std::transform(begin(),end(),other.begin(),g.begin(),op);
                                return g;
                            }
                        Grid operator+(const Grid& other) const {
                            return cwiseBinaryOp(other,[](auto&& a, auto&& b) {return a+b;});
                        }
                        Grid operator-(const Grid& other) const {
                            return cwiseBinaryOp(other,[](auto&& a, auto&& b) {return a-b;});
                        }

                        template <typename U>
                            Grid operator*(const U& v) const {
                                return cwiseUnaryOp([&v](auto&& a) {return v*a;});
                            }
                        template <typename U>
                            Grid operator/(const U& v) const {
                                return cwiseUnaryOp([&v](auto&& a) {return a/v;});
                            }
                        ////read only just over indices
                        //void loop(const std::function<void(const index_type&)>& f) const {
                        //    utils::multi_loop(m_shape,[&](auto&& v) {
                        //            f(v);
                        //            });
                        //}
                        //read only with values
                        void loop(const std::function<void(const index_type&, const Scalar&)>& f) const {
                            utils::multi_loop(shape(),[&](auto&& v) {
                                    f(v,(*this)(v));
                                    });
                        }
                        //write using index and current value
                        void loop_write(const std::function<Scalar(const index_type&, const Scalar&)>& f) {
                            utils::multi_loop(shape(),[&](auto&& v) {
                                    auto&& p = (*this)(v);
                                    p = f(v,p);
                                    });
                        }
                        //write using only index
                        void loop_write_idx(const std::function<Scalar(const index_type&)>& f) {
                            utils::multi_loop(shape(),[&](auto&& v) {
                                    (*this)(v) = f(v);
                                    });
                        }
                        //void cwiseUnaryOp(const std::function<Scalar(const Scalar&)>& f) {
                        //    std::transform(begin(),end(),begin(),f);
                        //}

                    public:
                        template <typename... Args>
                            static Grid Constant(const value_type& v,Args... args ) {
                                Grid g(args...);
                                g.set_constant(v);
                                return g;
                            }

                    private:
                        void init_data() {
                            set_constant(utils::internal::zero_value<T>());
                        }
                        storage_type m_storage;


                };
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

