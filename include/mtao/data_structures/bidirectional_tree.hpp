#pragma once
#include <memory>
#include <set>

namespace mtao::data_structures {

namespace internal {
    template<typename ValueType>
    struct BidirectionalTreeNode
      : public std::enable_shared_from_this<BidirectionalTreeNode<ValueType>> {
        using Ptr = std::shared_ptr<BidirectionalTreeNode>;
        using WeakPtr = std::weak_ptr<BidirectionalTreeNode>;
        ValueType value;
        std::set<WeakPtr> parent = {};
        std::set<Ptr, std::owner_less<Ptr>> children;

        bool is_leaf() const { return children.empty(); }
        void fix_children_parenting() {
            auto ptr = shared_from_this();
            for (auto &&c : children) {
                c.parent = ptr;
            }
        }

        void insert_above(Ptr value);
        void insert_above(ValueType &&value);
        WeakPtr unlink_parent();

      private:
        using std::enable_shared_from_this<
          BidirectionalTreeNode<ValueType>>::shared_from_this;
    };
}// namespace internal
template<typename ValueType = size_t>
struct BidirectionalTree {
    using NodeType = typename internal::BidirectionalTreeNode<ValueType>;
    // Nodes are weakly aware of their parents and take ownership of their
    // children
    std::set<typename NodeType::Ptr, std::owner_less<typename NodeType::Ptr>>
      roots;

    void add_root(ValueType &&value) { roots.emplace({ value, {}, {} }); }
};

namespace internal {
    template<typename ValueType>
    void BidirectionalTreeNode<ValueType>::fix_children_parenting() {
        auto ptr = shared_from_this();
        for (auto &&c : children) {
            c.parent = ptr;
        }
    }
    template<typename ValueType>
    void BidirectionalTreeNode<ValueType>::insert_above(Ptr ptr) {
        assert(ptr.parent.expired());
        if (!parent.expired(); parent.lock()) {
            auto handle = parent->children.extract(shared_from_this());
            ptr->children.emplace(std::move(handle));
        }
        ptr->parent = parent;
        parent = ptr;
    }
    template<typename ValueType>
    void BidirectionalTreeNode<ValueType>::insert_above(ValueType &&value) {
        Ptr np({ value, parent, { shared_from_this() } });
        parent = np;
    }
    template<typename ValueType>
    auto BidirectionalTreeNode<ValueType>::unlink_parent() -> WeakPtr {
        if (!parent.expired(); parent.lock()) {
            auto handle = parent->children.extract(shared_from_this());
            parent = {};
            return handle.value();
        } else {
            return {};
        }
    }
}// namespace internal
}// namespace mtao::data_structures
