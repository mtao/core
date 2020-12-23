
#pragma once
#include <compare>

#include "mtao/data_structures//directed_acyclic_graph.hpp"

namespace mtao::algebra {

template <typename Container, typename ComparisonOp>
data_structures::DirectedAcyclicGraph<size_t> partial_order_to_dag(
    const Container& container, ComparisonOp&& op = {}) {
    using DAG = data_structures::DirectedAcyclicGraph<size_t>;
    using NodeType = DAG::NodeType;

    DAG dag;

    // check that the return type of the operator is indeed a partial ordering
    using ValueType = typename Container::value_type;
    static_assert(
        std::is_same_v<std::partial_ordering, op(std::declval<ValueType>(),
                                                 std::declval<ValueType>())>);

    auto value_from_node = [&](const NodeType& node) -> const ValueType& {
        return container.at(node.value);
    };
    auto traverse_tree = [&](size_t index, const NodeType::Ptr& node_ptr) {};
    for (auto&& [index, p] : mtao::iterator::enumerate(container)) {
        NodeType::Ptr new_node{index, {}, {}};
        for (auto&& root : dag.roots) {
            std::partial_ordering order = op(p, value_from_node(*root));
            if (order < 0) {
                // new node is within this tree, lets dig in

                std::queue<NodeType::Ptr> queue(root.children.begin(),
                                                root.children.end());

            } else if (order > 0) {
                // new node is the parent
                auto handle = dag.roots.extract(root);
                NodeType::Ptr old_root = handle.value();
                handle.value() = new_node;
                handle.value()->children.emplace(std::move(old_root));
                dag.roots.emplace(handle);
            } else if (order == 0) {
                // they are equal just toss it and emit a warning
                spdlog::warn(
                    "partial_order_to_dag found identical elements, filter "
                    "equal elements before calling that function indices were "
                    "{} and {}",
                    index, root->value);
                break;
            } else {
                dag.roots.emplace(new_node);
            }
        }
    }
    return dat;
}

}  // namespace mtao::algebra
