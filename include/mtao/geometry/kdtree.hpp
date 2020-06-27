#pragma once
#include <Eigen/Core>
#include <array>
#include <memory>
#include <numeric>
#include <sstream>
#include <vector>

#include "mtao/types.h"

namespace mtao {
namespace geometry {
template <typename T, int D>
class KDTree;
template <typename T, int D, int Axis>
// class KDNode: public KDNodeBase<KDNode<T,D,Axis>> {
class KDNode {
   public:
    using Vec = mtao::Vector<T, D>;
    using NodeType = KDNode<T, D, Axis>;
    constexpr static int ChildAxis = (Axis + 1) % D;
    constexpr static int ParentAxis = (Axis - 1 + D) % D;
    using ChildNodeType = KDNode<T, D, ChildAxis>;
    using ParentNodeType = KDNode<T, D, ParentAxis>;
    // balanced construciton iterator, used for one constructor
    using BCIt = mtao::vector<size_t>::iterator;

    ~KDNode() = default;
    KDNode() = delete;
    KDNode(const KDNode&) = delete;
    KDNode(KDNode&&) = delete;
    KDNode& operator=(const KDNode&) = delete;
    KDNode& operator=(KDNode&&) = delete;
    KDNode(const KDTree<T, D>& tree, KDNode&& o)
        : m_tree(tree), m_index(o.m_index) {
        if (o.left()) {
            left() =
                std::make_unique<ChildNodeType>(tree, std::move(*o.left()));
        }
        if (o.right()) {
            right() =
                std::make_unique<ChildNodeType>(tree, std::move(*o.right()));
        }
        for(auto&& c: children) {
            c.reset();
        }
    }

    KDNode(const KDTree<T, D>& tree, int index)
        : m_tree(tree), m_index(index) {}
    KDNode(const KDTree<T, D>& tree, const BCIt& begin, const BCIt& end)
        : m_tree(tree) {
        assert(std::distance(begin, end) > 0);

        std::sort(begin, end, [&](size_t a, size_t b) {
            return point(a)(Axis) < point(b)(Axis);
        });
        BCIt mid = begin + std::distance(begin, end) / 2;
        BCIt right_start = mid + 1;
        m_index = *mid;
        if (std::distance(begin, mid) > 0) {
            left() = std::make_unique<ChildNodeType>(m_tree, begin, mid);
        }
        if (std::distance(right_start, end) > 0) {
            right() = std::make_unique<ChildNodeType>(m_tree, right_start, end);
        }
    }
    auto&& point() const { return point(m_index); }
    auto&& point(size_t idx) const { return m_tree.point(idx); }
    size_t index() const { return m_index; }

    template <typename Derived>
    bool operator<(const Eigen::MatrixBase<Derived>& p) const {
        // positive -> right
        // negative -> left
        return axis_dist(p) < 0;
    }
    template <typename Derived>
    T axis_dist(const Eigen::MatrixBase<Derived>& p) const {
        return p(Axis) - point()(Axis);
    }
    template <typename Derived>
    size_t axis_choice(const Eigen::MatrixBase<Derived>& p) const {
        return (*this < p) ? 0 : 1;
    }

    template <typename Derived>
    auto&& pick_child(const Eigen::MatrixBase<Derived>& p) {
        return m_children[axis_choice(p)];
    }
    template <typename Derived>
    auto&& pick_child(const Eigen::MatrixBase<Derived>& p) const {
        return m_children[axis_choice(p)];
    }

    void insert(size_t idx) {
        auto&& c = pick_child(point(idx));
        if (c) {
            c->insert(idx);
        } else {
            c = std::make_unique<ChildNodeType>(m_tree, idx);
        }
    }

    template <typename Derived, typename FilterFunc>
    void nearest_filtered(const Eigen::MatrixBase<Derived>& p, size_t& nearest_index,
                 T& nearest_dist, FilterFunc&& filter) const {
        if(!filter(point(),index())) {
            return;
        }
        T dist = (point() - p).norm();
        if (dist < nearest_dist) {
            nearest_index = index();
            nearest_dist = dist;
        }

        auto comp_child = [&](const std::unique_ptr<ChildNodeType>& c) {
            if (c) {
                c->nearest_filtered(p, nearest_index, nearest_dist,filter);
            }
        };
        T ad = axis_dist(p);
        bool in_nearband = nearest_dist >= std::abs(ad);

        //[  node     point  child]
        //[  node     child  point]
        //[  point    node   child]
        // axis nearest_dist is negative and considering going down right child
        if (ad >= 0 || in_nearband) {
            comp_child(right());
        }
        //[  point  child     node]
        //[  child  point     node]
        //[  child    node   point]
        // axis nearest_dist is positive and considering going down left child
        if (ad < 0 || in_nearband) {
            comp_child(left());
        }
    }

    template <typename Derived>
    void nearest(const Eigen::MatrixBase<Derived>& p, size_t& nearest_index,
                 T& nearest_dist) const {
        T dist = (point() - p).norm();
        if (dist < nearest_dist) {
            nearest_index = index();
            nearest_dist = dist;
        }

        auto comp_child = [&](const std::unique_ptr<ChildNodeType>& c) {
            if (c) {
                c->nearest(p, nearest_index, nearest_dist);
            }
        };
        T ad = axis_dist(p);
        bool in_nearband = nearest_dist >= std::abs(ad);

        //[  node     point  child]
        //[  node     child  point]
        //[  point    node   child]
        // axis nearest_dist is negative and considering going down right child
        if (ad >= 0 || in_nearband) {
            comp_child(right());
        }
        //[  point  child     node]
        //[  child  point     node]
        //[  child    node   point]
        // axis nearest_dist is positive and considering going down left child
        if (ad < 0 || in_nearband) {
            comp_child(left());
        }
    }

    std::unique_ptr<ChildNodeType>& left() { return m_children[0]; }
    const std::unique_ptr<ChildNodeType>& left() const { return m_children[0]; }

    std::unique_ptr<ChildNodeType>& right() { return m_children[1]; }
    const std::unique_ptr<ChildNodeType>& right() const {
        return m_children[1];
    }

    int max_depth() const {
        int depth = 0;
        if (left()) {
            depth = std::max(left()->max_depth() + 1, depth);
        }
        if (right()) {
            depth = std::max(right()->max_depth() + 1, depth);
        }
        return depth;
    }

    template <typename Func>
    void visitor(Func&& f, int depth = 0) const {
        f(point(), index(), depth);

        if (left()) {
            left()->visitor(f, depth + 1);
        }
        if (right()) {
            right()->visitor(f, depth + 1);
        }
    }
    template <typename ValueType, typename Reduce>
    ValueType accumulate(Reduce&& g, const ValueType& value,
                         int depth = 0) const {
        ValueType l = left() ? left()->accumulate(g, value, depth + 1) : value;
        ValueType r =
            right() ? right()->accumulate(g, value, depth + 1) : value;
        return g(point(), index(), depth, l, r);
    }

    operator std::string() const {
        std::stringstream ss;
        ss << m_index << "[";
        if (left()) {
            ss << std::string(*left());
        } else {
            ss << ".";
        }
        ss << " | ";
        if (right()) {
            ss << std::string(*right());
        } else {
            ss << ".";
        }
        ss << "]";

        return ss.str();
    }

   private:
    const KDTree<T, D>& m_tree;
    size_t m_index;
    std::array<std::unique_ptr<ChildNodeType>, 2> m_children;
};

template <typename T, int D>
class KDTree {
   public:
    using Vec = mtao::Vector<T, D>;
    constexpr static int Axis = 0;
    using NodeType = KDNode<T, D, 0>;
    const mtao::vector<Vec>& points() const { return m_points; }
    auto&& point(size_t idx) const { return m_points[idx]; }
    size_t size() const { return points().size(); }

    void rebalance() {
        std::vector<size_t> P(m_points.size());
        std::iota(P.begin(), P.end(), 0);
        m_node = std::make_unique<NodeType>(*this, P.begin(), P.end());
    }
    KDTree() = default;
    KDTree(const KDTree&) = delete;
    KDTree(KDTree&& o) {
        m_points = std::move(o.m_points);
        if (o.m_node) {
            m_node = std::make_unique<NodeType>(*this, std::move(*o.m_node));
        }
        return *this;
    }
    ~KDTree() = default;
    KDTree& operator=(const KDTree&) = delete;
    KDTree& operator=(KDTree&& o) {
        m_points = std::move(o.m_points);
        if (o.m_node) {
            m_node = std::make_unique<NodeType>(*this, std::move(*o.m_node));
        }
        return *this;
    }
    void reserve(size_t size) { m_points.reserve(size); }
    KDTree(const mtao::vector<Vec>& points) : m_points(points) { rebalance(); }
    size_t insert(const Vec& p) {
        size_t newidx = m_points.size();
        m_points.push_back(p);
        if (!m_node) {
            m_node = std::make_unique<NodeType>(*this, newidx);
        } else {
            m_node->insert(newidx);
        }
        return newidx;
    }
    size_t pruning_insertion(const Vec& p, T eps = T(1e-5)) {
        if (m_node) {
            auto [ni, d] = nearest(p);
            if (eps == 0) {
                if (point(ni) == p) {
                    return ni;
                }
            } else if (d < eps) {
                return ni;
            }
        }
        return insert(p);
    }
    template <typename Derived>
    std::tuple<size_t, T> nearest(
        const typename Eigen::MatrixBase<Derived>& p) const {
        size_t idx = 0;
        T d = std::numeric_limits<T>::max();
        assert(m_node);

        m_node->nearest(p, idx, d);
        return {idx, d};
    }
    template <typename Derived, typename Filter>
    std::tuple<size_t, T> nearest_filtered(
        const typename Eigen::MatrixBase<Derived>& p, Filter&& filter) const {
        size_t idx = 0;
        T d = std::numeric_limits<T>::max();
        assert(m_node);

        m_node->nearest_filtered(p, idx, d, filter);
        return {idx, d};
    }
    template <typename Derived>
    size_t nearest_index(const typename Eigen::MatrixBase<Derived>& p) const {
        return std::get<0>(nearest(p));
    }
    template <typename Derived, typename Filter>
    size_t nearest_index_filtered(const typename Eigen::MatrixBase<Derived>& p, Filter&& f) const {
        return std::get<0>(nearest_filtered(p,f));
    }
    template <typename Derived>
    size_t nearest_distance(
        const typename Eigen::MatrixBase<Derived>& p) const {
        return std::get<1>(nearest(p));
    }
    template <typename Derived>
    const Vec& nearest_point(
        const typename Eigen::MatrixBase<Derived>& p) const {
        return point(nearest_index(p));
    }
    template <typename Derived, typename FilterFunc>
    const Vec& nearest_point_filtered(
        const typename Eigen::MatrixBase<Derived>& p, FilterFunc&& f) const {
        return point(nearest_index_filtered(p,f));
    }

    operator std::string() const {
        std::string empty(".");
        return accumulate<std::string>(
            [](const Vec&, int index, int, const std::string& left_str,
               const std::string& right_str) {
                std::stringstream ss;
                ss << index << "[" << left_str << " | " << right_str << "]";
                return ss.str();
            },
            empty);
    }

    // simply runs f on each node breadth first, left then right
    // f has prototype (const Vec&,int, int) -> void; where Vec will be the
    // point and int will be the node index and depth respectively
    template <typename Func>
    void visitor(Func&& f) const {
        if (m_node) {
            return m_node->visitor(f);
        }
    }
    // runs a function on the graph doing the node,
    // g has prototype (const Vec&,int, int depth, ValueType l, ValueType r) ->
    // void; where Vec will be the point and int will be the node index, l,r are
    // the values from the left and right children, and depth is the depth in
    // teh graph
    template <typename ValueType, typename Reduce>
    ValueType accumulate(Reduce&& g, const ValueType& value) const {
        if (m_node) {
            return m_node->accumulate(g, value);
        } else {
            return value;
        }
    }

    int max_depth() const {
        return accumulate(
                   [](const Vec&, int, int depth, int left_depth,
                      int right_depth) {
                       return std::max<int>({depth, left_depth, right_depth});
                   },
                   0) +
               1;
        // if(m_node) {
        //    return m_node->max_depth() + 1;
        //}
        // return 0;
    }
    const std::unique_ptr<NodeType>& root() const { return m_node; }

   private:
    std::unique_ptr<NodeType> m_node;
    mtao::vector<Vec> m_points;
};

}  // namespace geometry
}  // namespace mtao

