#pragma once
#include "mtao/types.hpp"
#include "mtao/eigen/shape_checks.hpp"


// NOTE: if you wish to use custom attributes you want to include partio_loader_impl.hpp
// this design is so that we can avoid designing
namespace Partio {
class ParticlesData;
class ParticlesDataMutable;
}// namespace Partio
namespace mtao::geometry::point_cloud {

// my abstraction of partio files. currently does not take advantage of the ID field at all
struct PartioFileWriter {
  public:
    PartioFileWriter(const std::string &filename);
    ~PartioFileWriter();
    void set_positions(const mtao::ColVecs3d &P);
    void set_velocities(const mtao::ColVecs3d &V);
    void set_colors(const mtao::ColVecs4d &C);
    void set_radii(const mtao::VecXd &R);
    void set_ids(const mtao::VecXi &I);
    void update_size(int size);
    void write();

    template<eigen::concepts::ColVecsDCompatible T>
    void set_attribute(const std::string &name, const T &V);

    template<eigen::concepts::VecXCompatible T>
    void set_attribute(const std::string &name, const T &V);

  private:
    std::string _filename;
    Partio::ParticlesDataMutable *_handle;
};

struct PartioFileReader {
  public:
    PartioFileReader(const std::string &filename);
    ~PartioFileReader();
    mtao::ColVecs3d positions() const;
    mtao::ColVecs3d velocities() const;
    mtao::ColVecs4d colors() const;
    mtao::VecXd radii() const;
    mtao::VecXd densities() const;
    mtao::VecXi ids() const;


    template<typename T, int D>
    mtao::ColVectors<T, D> vector_attribute(const std::string &name) const;
    template<typename T>
    mtao::VectorX<T> attribute(const std::string &name) const;
    bool has_positions() const;
    bool has_velocities() const;
    bool has_colors() const;
    bool has_densities() const;
    bool has_radii() const;
    bool has_ids() const;
    std::vector<std::string> attributes() const;

    int particle_count() const;


    template<typename T, int D = 1>
    bool has_attribute(const std::string &name) const;


  private:
    Partio::ParticlesData *_handle;
};


mtao::ColVecs3d points_from_partio(const std::string &filename);
std::tuple<mtao::ColVecs3d, mtao::ColVecs3d> points_and_velocity_from_partio(const std::string &filename);


void write_to_partio(const std::string &filename, const mtao::ColVecs3d &P);
void write_to_partio(const std::string &filename, const mtao::ColVecs3d &P, const mtao::ColVecs3d &V);
}// namespace mtao::geometry::point_cloud
