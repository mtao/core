#pragma once

#include <mtao/geometry/mesh/triangle/mesh.h>
#include <map>
#include <memory>


class Shape {
  public:
    using REAL = mtao::geometry::mesh::triangle::REAL;
    using Mesh = mtao::geometry::mesh::triangle::Mesh;
    virtual Mesh boundary_geometry() = 0;


    void set_vert_indices(const std::map<int, int> &map);
    virtual int get_vert_index(int i) const;
    void set_edge_indices(const std::map<int, int> &map);
    virtual int get_edge_index(int i) const;
    void apply_edge_markers(mtao::VectorX<int> &V) const;
    void apply_vert_markers(mtao::VectorX<int> &V) const;
    virtual ~Shape();

  private:
    std::map<int, int> vert_marker_map;
    std::map<int, int> edge_marker_map;
};

class Transform : public Shape {

  public:
    Transform(const std::shared_ptr<Shape> &s);
    int get_vert_index(int i) const override;
    int get_edge_index(int i) const override;

  protected:
    std::shared_ptr<Shape> shape = nullptr;
};

class Translate : public Transform {
  public:
    Translate(const std::shared_ptr<Shape> &s, REAL tx, REAL ty);
    Mesh boundary_geometry() override;

  private:
    mtao::Vector<REAL, 2> t;
};
class Scale : public Transform {
  public:
    Scale(const std::shared_ptr<Shape> &sh, REAL sx, REAL sy);
    Scale(const std::shared_ptr<Shape> &sh, REAL s);
    Mesh boundary_geometry() override;

  private:
    mtao::Vector<REAL, 2> s;
};

class Square : public Shape {
  public:
    Mesh boundary_geometry() override;

    //vert marker: interior: 0, boundary: 1
    //edge indices: interior: 0, left: 1, bottom: 2, right: 3, top: 4
    Square(REAL r, int nx = 2, int ny = 2);
    Square() = default;
    Square(const Square &) = default;
    Square(Square &&) = default;
    Square &operator=(const Square &) = default;
    Square &operator=(Square &&) = default;

  private:
    REAL radius = 1;
    int nx = 2, ny = 2;
};


class Sphere : public Shape {
    //vert marker: interior: 0, boundary: 1
    //edge marker: interior: 0, boundary: 1
  public:
    Mesh boundary_geometry() override;
    Sphere() = default;
    Sphere(REAL r, int vertcount = 10);
    Sphere(const Sphere &) = default;
    Sphere(Sphere &&) = default;
    Sphere &operator=(const Sphere &) = default;
    Sphere &operator=(Sphere &&) = default;

  private:
    REAL radius = 1;
    int vertcount = 10;
};
