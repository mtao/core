#ifndef OCTREE_H
#define OCTREE_H

template<typename Scalar, int D>
class OctreeNode {
  public:
  private:
};

template<typename Scalar, int D>
class Octree : public OctreeNode<Scalar, D> {
  public:
    using NodeType = OctreeNode<Scalar, D>;
};
#endif//OCTREE_H
