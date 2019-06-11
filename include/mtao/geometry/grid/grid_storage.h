#pragma once
#include <stdexcept>
#include "mtao/types.hpp"
#include "grid_utils.h"

namespace mtao {
    namespace geometry {
        namespace grid {
            namespace internal {
                template <typename T, typename Allocator = ::mtao::allocator<T>, bool CopyOnResize=false >
                    class grid_storage {
                        public:
                            using value_type = T;
                            using iterator_type = T*;
                            using const_iterator_type = const T*;
                            using allocator_type = Allocator;
                            T* data() { return m_data; }
                            const T* data() const {return m_data;}
                            iterator_type begin() { return m_data; }
                            iterator_type end() { return m_data+m_size; }
                            const_iterator_type begin() const { return m_data; }
                            const_iterator_type end() const { return m_data+m_size; }
                            grid_storage() {
                            }
                            grid_storage(size_t s) {
                                resize(s);
                                m_size = s;
                            }
                            grid_storage(const grid_storage& other): m_size(other.size()) {
                                resize(size());
                                std::copy(other.begin(),other.end(),begin());

                            }
                            grid_storage(grid_storage&& other): m_size(other.m_size), m_data(other.m_data) {
                                other.m_data = nullptr;
                                other.m_size = 0;
                            }

                            grid_storage& operator=(const grid_storage& other) {
                                if(other.size() != size()) {
                                    m_size = other.size();
                                    resize(size());
                                }
                                std::copy(other.begin(),other.end(),begin());
                                return *this;
                            }
                            grid_storage& operator=(grid_storage&& other) {
                                m_size = other.m_size;
                                m_data = other.m_data;
                                other.m_data = nullptr;
                                other.m_size = 0;
                                return *this;
                            }


                            ~grid_storage() { 
                                allocator_type a;
                                a.deallocate(m_data,size());
                            }
                            bool empty() const { return m_data == nullptr; }
                            T& at(size_t idx) {
                                if(!(idx < size())) {
                                    std::stringstream ss;
                                    ss << "Grid accessed out of range: " << idx << "/" << size();
                                    throw std::out_of_range(ss.str());
                                }
                                return m_data[idx];
                            }
                            const T& at(size_t idx) const {
                                if(!(idx < size())) {
                                    std::stringstream ss;
                                    ss << "Grid accessed out of range: " << idx << "/" << size();
                                    throw std::out_of_range(ss.str());
                                }
                                return m_data[idx];
                            }
                            T& operator()(size_t idx) {
                                assert(idx < size());
                                return m_data[idx];
                            }
                            const T& operator()(size_t idx) const {
                                assert(idx < size());
                                return m_data[idx];
                            }

                            void resize(size_t size) {
                                if(size == 0) {
                                    m_size = 0;
                                    m_data = nullptr;
                                    return;
                                }
                                allocator_type a;
                                value_type* new_data = a.allocate(size);
                                if(!empty()) {
                                    if(CopyOnResize) {
                                        size_t copy_size = std::min(size,this->size());
                                        std::copy(m_data,m_data+copy_size,new_data);
                                        if(copy_size < this->size()) {
                                            for(size_t i = copy_size; i < size; ++i) {
                                                a.construct(m_data+i,T{});
                                            }

                                        }
                                    }
                                    a.deallocate(m_data,size);
                                }
                                m_data = new_data;
                            }
                            size_t size() const {
                        return m_size;
                    }
                private:
                    size_t m_size = 0;
                    value_type* m_data = nullptr;
            };
    }
        }
    }
}
