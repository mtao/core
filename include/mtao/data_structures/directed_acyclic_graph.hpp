#pragma once
#include <memory>
#include <set>

namespace mtao::data_structures {

namespace internal {
template <typename ValueType>
struct DirectedAcyclicGraphNode
    : public std::enable_shared_from_this<DirectedAcyclicGraphNode<ValueType>> {
    using Ptr = std::shared_ptr<DirectedAcyclicGraphNode>;
    using PtrSet = std::set<Ptr, std::owner_less<Ptr>>;
    using value_type = ValueType;
    ValueType value;
    PtrSet children;

    bool is_leaf() const { return children.empty(); }

    void insert_child(const Ptr& ptr);
    void insert_between_child(Ptr ptr,
                              typename decltype(children)::iterator it);

   private:
    using std::enable_shared_from_this<
        DirectedAcyclicGraphNode<ValueType>>::shared_from_this;
};
}  // namespace internal
template <typename ValueType = size_t>
struct DirectedAcyclicGraph {
    using NodeType = typename internal::DirectedAcyclicGraphNode<ValueType>;
    using NodePtrSet = typename NodeType::PtrSet;
    // Nodes are weakly aware of their parents and take ownership of their
    // children
    NodePtrSet roots;

    void add_root(ValueType&& value) { roots.emplace({value, {}, {}}); }

    NodePtrSet nodes() const;
};

namespace internal {
template <typename ValueType>
void DirectedAcyclicGraphNode<ValueType>::insert_child(const Ptr& ptr) {
    children.emplace(ptr);
}
template <typename ValueType>
void DirectedAcyclicGraphNode<ValueType>::insert_between_child(
    Ptr ptr, typename decltype(children)::iterator it) {
    auto handle = children.extract(it);
    ptr->children.emplace(handle.value());
    std::swap(handle.value(), ptr);
    children.insert(std::move(handle));
}

template <typename NodePtr, typename NodePtrStorage>
void dag_node_collector(const NodePtr& n, NodePtrStorage& nodes) {
    if (!nodes.contains(n)) {
        nodes.insert(n);
        for (auto&& c : n->children) {
            dag_node_collector(c, nodes);
        }
    }
}

}  // namespace internal

template <typename ValueType>
auto DirectedAcyclicGraph<ValueType>::nodes() const -> NodePtrSet {
    NodePtrSet ret;
    for (auto&& r : roots) {
        internal::dag_node_collector(r, ret);
    }
    return ret;
}
}  // namespace mtao::data_structures
