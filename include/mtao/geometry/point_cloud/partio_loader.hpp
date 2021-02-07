#pragma once
#include "mtao/types.hpp"


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
    mtao::VecXi ids() const;
    bool has_positions() const;
    bool has_velocities() const;
    bool has_colors() const;
    bool has_radii() const;
    bool has_ids() const;

    int particle_count() const;


  private:
    Partio::ParticlesData *_handle;
};


mtao::ColVecs3d points_from_partio(const std::string &filename);
std::tuple<mtao::ColVecs3d, mtao::ColVecs3d> points_and_velocity_from_partio(const std::string &filename);


void write_to_partio(const std::string &filename, const mtao::ColVecs3d &P);
void write_to_partio(const std::string &filename, const mtao::ColVecs3d &P, const mtao::ColVecs3d &V);
}// namespace mtao::geometry::point_cloud
