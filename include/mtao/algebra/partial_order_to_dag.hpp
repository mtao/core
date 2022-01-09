#pragma once
#include <spdlog/spdlog.h>

#include <compare>
#include <queue>

#include "mtao/data_structures//directed_acyclic_graph.hpp"
#include "mtao/iterator/enumerate.hpp"

namespace mtao::algebra {

template<typename Container, typename ComparisonOp>
data_structures::DirectedAcyclicGraph<size_t> partial_order_to_dag(
  const Container &container,
  ComparisonOp &&op = {}) {
    using DAG = data_structures::DirectedAcyclicGraph<size_t>;
    using NodeType = DAG::NodeType;

    DAG dag;

    // check that the return type of the operator is indeed a partial ordering
    using ValueType = typename Container::value_type;
    static_assert(std::is_same_v<std::partial_ordering,
                                 decltype(op(std::declval<ValueType>(),
                                             std::declval<ValueType>()))>);

    auto value_from_node = [&](const NodeType &node) -> const ValueType & {
        return container.at(node.value);
    };
    // auto traverse_tree = [&](size_t index, const NodeType::Ptr& node_ptr) {};
    for (auto &&[index, p] : mtao::iterator::enumerate(container)) {
        // This named variable seems required for clang 
        // otherwise clang emits a reference_to_local_in_enclosing_context
        const auto& pp = p;
        const auto& indexindex= index;
        NodeType::Ptr new_node = std::make_shared<NodeType>();
        new_node->value = index;

        // through all of the traversals we might redo comparsions which might
        // be costly, so lets keep track of the ones we've seen
        std::set<typename NodeType::Ptr,
                 std::owner_less<typename NodeType::Ptr>>
          seen;
        seen.emplace(new_node);
        auto scan_for_children = [&](const NodeType::Ptr &ptr) {
            std::queue<NodeType::Ptr> queue({ ptr });
            while (!queue.empty()) {
                auto ptr = queue.front();
                queue.pop();
                std::partial_ordering order = op(pp, value_from_node(*ptr));
                if (order > 0) {
                    new_node->children.emplace(ptr);
                } else if (order != 0) {
                    for (auto &&c : ptr->children) {
                        if (!seen.contains(c)) {
                            queue.emplace(c);
                            seen.emplace(ptr);
                        }
                    }
                } else {
                    spdlog::error(
                      "partial_order_to_dag::scan_for_children hit a non-dag "
                      "relationship {}({}) {}({})",
                      pp,
                      indexindex,
                      value_from_node(*ptr),
                      ptr->value);
                }
            }
        };
        if (dag.roots.empty()) {
            dag.roots.emplace(new_node);
            continue;
        } else {
            bool has_parent = false;
            for (auto &&root : dag.roots) {
                const ValueType &root_val = value_from_node(*root);

                std::partial_ordering order = op(p, root_val);
                if (order < 0) {
                    // new node is within this tree, lets dig in
                    has_parent = true;

                    std::queue<NodeType::Ptr> queue({ root });

                    while (!queue.empty()) {
                        auto ptr = queue.front();
                        queue.pop();
                        seen.emplace(ptr);
                        bool potential_child_leaf = true;
                        for (auto it = ptr->children.begin();
                             it != ptr->children.end();
                             ++it) {
                            std::partial_ordering order =
                              op(p, value_from_node(**it));
                            if (order < 0) {
                                queue.emplace(*it);
                                potential_child_leaf = false;
                            } else if (order > 0) {
                                ptr->insert_between_child(new_node, it);
                                potential_child_leaf = false;
                            } else if (order != 0) {
                                scan_for_children(*it);
                            }
                        }
                        if (potential_child_leaf) {
                            ptr->insert_child(new_node);
                        }
                    }

                } else if (order > 0) {
                    // new node is the parent
                    auto handle = dag.roots.extract(root);
                    NodeType::Ptr old_root = handle.value();
                    handle.value() = new_node;
                    handle.value()->children.emplace(std::move(old_root));
                    dag.roots.insert(std::move(handle));
                } else if (order != 0) {
                    scan_for_children(root);
                } else {
                    // they are equal just toss it and emit a warning
                    spdlog::warn(
                      "partial_order_to_dag found identical elements, "
                      "filter "
                      "equal elements before calling that function "
                      "indices "
                      "were "
                      "{} and {}",
                      index,
                      root->value);
                    break;
                }
            }
            if (!has_parent) {
                dag.roots.emplace(new_node);
            }
        }
    }
    return dag;
}

}// namespace mtao::algebra
