#ifndef INDEX_MAP_H
#define INDEX_MAP_H

//#include <array>
#include "compat.h"
#include <map>
#include "arguments.h"



//TODO: Test remapping...

namespace mtao {

    template <int D>
        class IndexMapNode;

    template <int D>
        class IndexMap {
            public:
                using IndexType = mtao::compat::array<int,D>;
                int add(const IndexType& idx) {
                    int retval = 0;
                    if(!m_root.add(idx,m_size,&retval)) {
                        m_size++;
                    }
                    return retval;
                }
                int get(const IndexType& idx) const {
                    return m_root.get(idx);
                }
                bool has(const IndexType& idx) const {
                    return m_root.has(idx);
                }
                template <typename... Args>
                    int add(Args... args) {
                        IndexType idx = mtao::arguments_to_array<IndexType>(args...);
                        return add(idx);
                    }
                template <typename... Args>
                    int get(Args... args) const {
                        IndexType idx = mtao::arguments_to_array<IndexType>(args...);
                        return get(idx);
                    }
                template <typename... Args>
                    bool has(Args... args) const {
                        IndexType idx = mtao::arguments_to_array<IndexType>(args...);
                        return has(idx);
                    }
                void reindex() {
                    int index=0;
                    m_root.reindex(&index);
                }
                void clear() {m_root.clear(); m_size = 0;}

                std::map<int,int> reindex_with_mapping() {
                    int index=0;
                    std::map<int,int> remap;
                    m_root.reindex(remap,&index);
                    return remap;
                }
                size_t size() const {return m_size;}

                class IndexMapIterator;
                class IndexMapIteratorInternal;
                struct IndexMapDataContainer {
                    public:
                        mtao::compat::array<int,D> coord;
                        int index;
                };
                IndexMapNode<D-1>& root() {return m_root;}
                const IndexMapNode<D-1>& root() const {return m_root;}

                IndexMapIterator begin() { return IndexMapIterator(*this); }
                IndexMapIterator end() { return IndexMapIterator(*this,0); }
                IndexMapIterator begin() const { return IndexMapIterator(*this); }
                IndexMapIterator end() const { return IndexMapIterator(*this,0); }

                //IndexMapIterator find(const IndexType& idx) const {
                //    return IndexMapIterator(m_root,idx);//m_root.find(idx);
                //}
                //template <typename... Args>
                //    IndexMapIterator add(Args... args) const {
                //        IndexType idx = mtao::arguments_to_array<IndexType>(args...);
                //        return find(idx);
                //    }
            private:
                IndexMapNode<D-1> m_root;
                size_t m_size = 0;
        };


    template <int D>
        class IndexMapNode {
            public:
                typedef IndexMapNode<D-1> ChildNodeType;
                typedef std::map<int,ChildNodeType> MapType;
                typedef typename MapType::iterator iterator;
                typedef typename MapType::const_iterator const_iterator;

                IndexMapNode() {}
                template <typename IdxType>
                    IndexMapNode(const IdxType& idx, int value) {
                        m_map.emplace(idx[D],ChildNodeType(idx,value));
                    }
                void reindex(int* index);
                void reindex(std::map<int,int>& remap, int* index);
                template <typename IdxType>
                    bool add(const IdxType& idx, size_t m_size, int* retval);
                template <typename IdxType>
                    int get(const IdxType& idx) const;
                template <typename IdxType>
                    bool has(const IdxType& idx) const;
                void clear() {m_map.clear();}

                iterator begin() {return m_map.begin();}
                const_iterator begin() const {return m_map.begin();}
                //const_iterator cbegin() const {return m_map.begin();}
                iterator end() {return m_map.end();}
                const_iterator end() const {return m_map.end();}
                //const_iterator cend() const {return m_map.end();}
            private:
                MapType m_map;

        };


    template <>
        class IndexMapNode<0> {
            public:
                typedef std::map<int,int> MapType;
                typedef typename MapType::iterator iterator;
                typedef typename MapType::const_iterator const_iterator;
                const static int D = 0;
                IndexMapNode() {}
                template <typename IdxType>
                    IndexMapNode(const IdxType& idx, int value) {
                        m_values[idx[D]] = value;
                    }
                template <typename IdxType>
                    bool add(const IdxType& idx, size_t m_size, int* retval);

                template <typename IdxType>
                    int get(const IdxType& idx) const {
                        return m_values.at(idx[D]);
                    }
                template <typename IdxType>
                    bool has(const IdxType& idx) const {
                        return !(m_values.find(idx[D]) == m_values.end());
                    }

                void reindex(int* index) {
                    for(auto&& i: m_values) {
                        i.second = (*index)++;
                    }
                }
                void reindex(std::map<int,int>& remap, int* index) {
                    for(auto&& i: m_values) {
                        remap[i.second] = *index;
                        i.second = (*index)++;
                    }
                }
                void clear() {m_values.clear();}
                iterator begin() {return m_values.begin();}
                const_iterator begin() const {return m_values.begin();}
                iterator end() {return m_values.end();}
                const_iterator end() const {return m_values.end();}

            private:
                MapType m_values;
        };



    template <typename IdxType>
        bool IndexMapNode<0>::add(const IdxType& idx,size_t m_size,int* retval) {
            auto it = m_values.find(idx[D]);
            if(it == m_values.end()) {
                m_values[idx[D]] = m_size;
                *retval = m_size;
                return false;
            } else {
                *retval = it->second;
                return true;
            }
        }

    template <int D>
        void IndexMapNode<D>::reindex(int* index) {
            for(auto&& i: m_map) {
                i.second.reindex(index);
            }
        }
    template <int D>
        void IndexMapNode<D>::reindex(std::map<int,int>& remap, int* index) {
            for(auto&& i: m_map) {
                i.second.reindex(remap, index);
            }
        }


    template <int D>
        template <typename IdxType>
        bool IndexMapNode<D>::add(const IdxType& idx, size_t m_size, int* retval) {
            auto it = m_map.find(idx[D]);
            if(it == m_map.end()) {
                auto& child_map = m_map[idx[D]];
                child_map.add(idx,m_size,retval);
                return false;
                //Add new submap
            } else {
                return it->second.add(idx,m_size,retval);
            }
        }

    template <int D>
        template <typename IdxType>
        int IndexMapNode<D>::get(const IdxType& idx) const {
            return m_map.at(idx[D]).get(idx);
        }
    template <int D>
        template <typename IdxType>
        bool IndexMapNode<D>::has(const IdxType& idx) const {
            auto&& it = m_map.find(idx[D]);
            if(it == m_map.end()) {
                return false;
            } else {
                return it->second.has(idx);
            }

        }

    //template <int D>
    //class MultiIndexMap {
    //    public:
    //        using IndexType = std::vector<int>;
    //        size_t get(const IndexType& idx) {
    //            int retval = 0;
    //            if(!m_root.get(idx,m_size,&retval)) {
    //                m_size++;
    //            }
    //            return retval;
    //        }
    //        template <typename... Args>
    //            size_t get(Args... args) {
    //                IndexType idx = mtao::arguments_to_array<IndexType>(args...);
    //                return get(idx);
    //            }
    //        size_t size() const {return m_size;}
    //    private:
    //        IndexMapNode<D-1> m_root;
    //        size_t m_size = 0;
    //};

}
#include "index_map_iterator.h"
#endif
