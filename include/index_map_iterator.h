#ifdef INDEX_MAP_H//Only use this file if incldued in index map
#ifndef INDEX_MAP_ITERATOR_H
#define INDEX_MAP_ITERATOR_H
template <int D>
class IndexMap<D>::IndexMapIterator {
    public:
        IndexMapDataContainer m_value;
        IndexMapIterator(const IndexMap<D>& map) {
            m_root.setIter(map.root(),coord(),&index());
        }
        IndexMapIterator(const IndexMap<D>& map, bool) {
            m_root.setIter(map.root(),coord(),&index());
            m_root.setEnd();
        }
        
        bool operator==(IndexMapIterator& other) const {
            return m_root == other.m_root;
        }
        operator bool() const {
            return !m_root.atEnd();
        }



        //int operator*() const {
        //    return *m_value;
        //}
        IndexMapIterator& operator++() {
            m_root.increment(coord(),&index());
            return *this;
        }

        IndexMapDataContainer& operator*() {
            return m_value;
        }
        mtao::compat::array<int,D>& coord() {return m_value.coord;}
        int& index() {return m_value.index;}
        private:
        typename IndexMap<D-1>::IndexMapIteratorInternal m_root;

};

template <int D>
class IndexMap<D>::IndexMapIteratorInternal {
    public:
        typedef typename IndexMapNode<D>::const_iterator InternalIteratorInternal;
        int operator*() const {
            return *child;
        }
        bool operator==(const IndexMapIteratorInternal& other) const {
            return iter == other.iter && child == other.child;
        }
        template <typename IndexType>
            void increment(IndexType& coord, int* index) {
                if(atEnd()) return;
                child.increment(coord,index);
                if(child.atEnd()) {
                    iter++;
                    if(!atEnd()) {
                        coord[D] = iter->first;
                        child.setIter(iter->second, coord, index);
                    }
                }
            }
        bool atEnd() const {
            return iter == end;
        }
        void setEnd() {
            iter = end;
        }
        template <typename IndexType>
            void setIter(const IndexMapNode<D> & map, IndexType& coord, int* index) {
                iter = map.begin();
                end = map.end();
                if(atEnd()) {
                    return;
                }
                coord[D] = iter->first;
                child.setIter(iter->second,coord,index);
            }
    private:
        InternalIteratorInternal iter,end;
        typename IndexMap<D-1>::IndexMapIteratorInternal child;
};
template <>
class IndexMap<0>::IndexMapIteratorInternal {
    public:
        typedef typename IndexMapNode<0>::const_iterator InternalIteratorInternal;
        int operator*() const {
            return iter->second;
        }
        bool operator==(const IndexMapIteratorInternal& other) const {
            return iter == other.iter;
        }
        template <typename IndexType>
            void increment(IndexType& coord, int* index) {
                if(atEnd()) return;
                iter++;
                if(!atEnd()) {
                    *index = iter->second;
                    coord[0] = iter->first;
                }
            }
        bool atEnd() const {
            return iter == end;
        }
        void setEnd() {
            iter = end;
        }
        template <typename IndexType>
            void setIter(const IndexMapNode<0>& map, IndexType& coord, int* index) {
                iter = map.begin();
                end = map.end();
                if(atEnd()) {
                    return;
                }
                *index = iter->second;
                coord[0] = iter->first;
            }
    private:
        InternalIteratorInternal iter,end;
};




#endif//INDEX_MAP_ITERATOR_H
#endif//INDEX_MAP_H
