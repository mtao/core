#pragma once
#include "mtao/types.hpp"
#include "mtao/data_structures/disjoint_set.hpp"

//Takes input signed geometry comprised of half-structures and generates cells

namespace mtao::geometry {
template<typename InternalPredicate, typename IsDualPredicate, typename SignedVolumePredicate = mtao::VecXd>
class NestedGeometryCombiner : public DisjointSet<int> {

  public:
    NestedGeometryCombiner(const InternalPredicate &ip, const IsDualPredicate &idp, const SignedVolumePredicate &svp) : is_inside(ip), is_dual(idp), volume(vp) {
        add_node(-1);
        for (int i = 0; i < size(); ++i) {
            add_node(i);
        }
    }

    std::array<std::set<int>, 2> volume_sign_bins() const;
    void compute();

    size_t size() const { return _nodes.size(); }

  private:
    InternalPredicate is_inside;
    IsDualPredicate is_dual;
    SignedVolumePredicate volume;
};
template<typename InternalPredicate, typename SignedVolumePredicate>
std::array<std::set<size_t>, 2> NestedGeometryCombiner<InternalPredicate, SignedVolumePredicate>::volume_sign_bins() const {

    std::array<std::set<int>, 2> ret;
    auto &[positive, negative] = ret;
    for (int i = 0; i < size(); ++i) {
        double vol = volume(i);
        if (vol > 0) {
            positive.insert(i);
        } else if (vol < 0) {
            negative.insert(i);
        } else {
            join(i, -1);// i parent is -1
        }
        return ret;
    }
    template<typename InternalPredicate, typename SignedVolumePredicate>
    void NestedGeometryCombiner<InternalPredicate, SignedVolumePredicate>::compute() {
        auto [outer, inner] = volume_sign_bins();

        for (auto &&oidx : outer) {
            for (auto &iidx : inner) {
                if (is_dual(oidx, iidx)) {
                    continue;
                }
            }
        }
    }
