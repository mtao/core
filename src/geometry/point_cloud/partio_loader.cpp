#include <Partio.h>
#include <spdlog/spdlog.h>
#include "mtao/geometry/point_cloud/partio_loader.hpp"
#include <fmt/format.h>
namespace mtao::geometry::point_cloud

{


PartioFileWriter::PartioFileWriter(const std::string &filename) : _filename(filename), _handle(Partio::create()) {
    if (!_handle) throw std::invalid_argument(fmt::format("failed to open {}", filename));
}
PartioFileWriter::~PartioFileWriter() {
    write();
    _handle->release();
}

void PartioFileWriter::update_size(int size) {

    int cur = _handle->numParticles();
    if (cur < size) {
        _handle->addParticles(size - cur);
    }
}
void PartioFileWriter::set_positions(const mtao::ColVecs3d &P) {

    Partio::ParticleAttribute attr = _handle->addAttribute(
      "position", Partio::ParticleAttributeType::VECTOR, 3);

    update_size(P.cols());

    auto it = _handle->begin();
    for (int j = 0; j < P.cols(); ++j, ++it) {
        auto p = P.col(j);
        float *dat = _handle->dataWrite<float>(attr, it.index);
        Eigen::Map<mtao::Vec3f>{ dat } = p.cast<float>();
    }
}
void PartioFileWriter::set_velocities(const mtao::ColVecs3d &V) {
    Partio::ParticleAttribute attr = _handle->addAttribute(
      "velocity", Partio::ParticleAttributeType::VECTOR, 3);
    update_size(V.cols());

    auto it = _handle->begin();
    for (int j = 0; j < V.cols(); ++j, ++it) {
        auto p = V.col(j);
        float *dat = _handle->dataWrite<float>(attr, it.index);
        Eigen::Map<mtao::Vec3f>{ dat } = p.cast<float>();
    }
}
void PartioFileWriter::set_colors(const mtao::ColVecs4d &C) {
    Partio::ParticleAttribute attr = _handle->addAttribute(
      "color", Partio::ParticleAttributeType::FLOAT, 4);
    update_size(C.cols());
    auto it = _handle->begin();
    for (int j = 0; j < C.cols(); ++j, ++it) {
        auto p = C.col(j);
        float *dat = _handle->dataWrite<float>(attr, it.index);
        Eigen::Map<mtao::Vec4f>{ dat } = p.cast<float>();
    }
}
void PartioFileWriter::set_ids(const mtao::VecXi &I) {
    Partio::ParticleAttribute attr = _handle->addAttribute(
      "id", Partio::ParticleAttributeType::INT, 1);
    update_size(I.size());

    auto it = _handle->begin();
    for (int j = 0; j < I.size(); ++j, ++it) {
        int *dat = _handle->dataWrite<int>(attr, it.index);
        *dat = I(j);
    }
}
void PartioFileWriter::set_radii(const mtao::VecXd &I) {
    Partio::ParticleAttribute attr = _handle->addAttribute(
      "radius", Partio::ParticleAttributeType::FLOAT, 1);
    update_size(I.size());

    auto it = _handle->begin();
    for (int j = 0; j < I.size(); ++j, ++it) {
        float *dat = _handle->dataWrite<float>(attr, it.index);
        *dat = I(j);
    }
}

void PartioFileWriter::write() {

    Partio::write(_filename.c_str(), *_handle);

}

PartioFileReader::PartioFileReader(const std::string &filename) : _handle(Partio::read(filename.c_str())) {
    if (!_handle) throw std::invalid_argument(fmt::format("failed to open {}", filename));
}
PartioFileReader::~PartioFileReader() {
    _handle->release();
}
int PartioFileReader::particle_count() const {
    return _handle->numParticles();
}
mtao::ColVecs3d PartioFileReader::positions() const {
    Partio::ParticleAttribute attr;
    if (!_handle->attributeInfo("position", attr) || attr.type != Partio::VECTOR || attr.count != 3)
        throw std::invalid_argument("failed to get positions as a vector of size 3");

    mtao::ColVecs3d R(3, particle_count());

    auto iterator = _handle->begin();
    Partio::ParticleAccessor acc(attr);
    iterator.addAccessor(acc);

    int cnt = 0;
    for (auto it = _handle->begin(); it != _handle->end(); ++it) {
        float *data = acc.raw<float>(it);
        R.col(cnt++) = Eigen::Map<const mtao::Vec3f>{ data }.cast<double>();
    }
    return R;
}
mtao::ColVecs3d PartioFileReader::velocities() const {
    Partio::ParticleAttribute attr;
    if (!_handle->attributeInfo("velocity", attr) || attr.type != Partio::VECTOR || attr.count != 3)
        throw std::invalid_argument("failed to get velocities as a vector of size 3");
    mtao::ColVecs3d R(3, particle_count());
    auto iterator = _handle->begin();
    Partio::ParticleAccessor acc(attr);
    iterator.addAccessor(acc);

    int cnt = 0;
    for (auto it = _handle->begin(); it != _handle->end(); ++it) {
        float *data = acc.raw<float>(it);
        R.col(cnt++) = Eigen::Map<const mtao::Vec3f>{ data }.cast<double>();
    }
    return R;
}
mtao::ColVecs4d PartioFileReader::colors() const {
    Partio::ParticleAttribute attr;
    if (!_handle->attributeInfo("color", attr) || attr.type != Partio::FLOAT || attr.count != 4)
        throw std::invalid_argument("failed to get colors as a float of size 4");
    mtao::ColVecs4d R(4, particle_count());
    auto iterator = _handle->begin();
    Partio::ParticleAccessor acc(attr);
    iterator.addAccessor(acc);

    int cnt = 0;
    for (auto it = _handle->begin(); it != _handle->end(); ++it) {
        float *data = acc.raw<float>(it);
        R.col(cnt++) = Eigen::Map<const mtao::Vec4f>{ data }.cast<double>();
    }
    return R;
}
mtao::VecXi PartioFileReader::ids() const {
    Partio::ParticleAttribute attr;
    if (!_handle->attributeInfo("id", attr) || attr.type != Partio::INT || attr.count != 1)
        throw std::invalid_argument("failed to get ids as a int of size 1");
    mtao::VecXi R(particle_count());
    auto iterator = _handle->begin();
    Partio::ParticleAccessor acc(attr);
    iterator.addAccessor(acc);

    int cnt = 0;
    for (auto it = _handle->begin(); it != _handle->end(); ++it) {
        int *data = acc.raw<int>(it);
        R(cnt++) = *data;
    }
    return R;
}
mtao::VecXd PartioFileReader::radii() const {
    Partio::ParticleAttribute attr;
    if (!_handle->attributeInfo("radii", attr) || attr.type != Partio::FLOAT || attr.count != 1)
        throw std::invalid_argument("failed to get radiias a float of size 1");
    mtao::VecXd R(particle_count());
    auto iterator = _handle->begin();
    Partio::ParticleAccessor acc(attr);
    iterator.addAccessor(acc);

    int cnt = 0;
    for (auto it = _handle->begin(); it != _handle->end(); ++it) {
        float *data = acc.raw<float>(it);
        R(cnt++) = *data;
    }
    return R;
}

bool PartioFileReader::has_positions() const {
    Partio::ParticleAttribute attr;
    if (!_handle->attributeInfo("position", attr) || attr.type != Partio::VECTOR || attr.count != 3)
        return false;
    return true;
}
bool PartioFileReader::has_velocities() const {
    Partio::ParticleAttribute attr;
    if (!_handle->attributeInfo("velocity", attr) || attr.type != Partio::VECTOR || attr.count != 3)
        return false;
    return true;
}
bool PartioFileReader::has_colors() const {
    Partio::ParticleAttribute attr;
    if (!_handle->attributeInfo("color", attr) || attr.type != Partio::FLOAT || attr.count != 4)
        return false;
    return true;
}
bool PartioFileReader::has_ids() const {
    Partio::ParticleAttribute attr;
    if (!_handle->attributeInfo("id", attr) || attr.type != Partio::INT || attr.count != 1)
        return false;
    return true;
}

bool PartioFileReader::has_radii() const {
    Partio::ParticleAttribute attr;
    if (!_handle->attributeInfo("radii", attr) || attr.type != Partio::FLOAT || attr.count != 1)
        return false;
    return true;
}

mtao::ColVecs3d points_from_partio(const std::string &filename) {
    return PartioFileReader(filename).positions();
}
std::tuple<mtao::ColVecs3d, mtao::ColVecs3d> points_and_velocity_from_partio(const std::string &filename) {
    PartioFileReader r(filename);
    return { r.positions(), r.velocities() };
}

void write_to_partio(const std::string &filename, const mtao::ColVecs3d &P) {
    PartioFileWriter w(filename);
    w.set_positions(P);
}
void write_to_partio(const std::string &filename, const mtao::ColVecs3d &P, const mtao::ColVecs3d &V) {
    PartioFileWriter writer(filename);
    writer.set_positions(P);
    writer.set_velocities(V);
}
}// namespace mtao::geometry::point_cloud
