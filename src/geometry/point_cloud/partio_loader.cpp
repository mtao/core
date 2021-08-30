#include "mtao/geometry/point_cloud/partio_loader_impl.hpp"
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

    set_attribute("position", P.cast<float>());
}
void PartioFileWriter::set_velocities(const mtao::ColVecs3d &V) {
    set_attribute("velocity", V.cast<float>());
}
void PartioFileWriter::set_colors(const mtao::ColVecs4d &C) {
    set_attribute("color", C.cast<float>());
}
void PartioFileWriter::set_ids(const mtao::VecXi &I) {
    set_attribute("id", I);
}
void PartioFileWriter::set_radii(const mtao::VecXd &I) {
    set_attribute("radius", I);
}
void PartioFileWriter::set_densities(const mtao::VecXd &I) {
    set_attribute("density", I);
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


std::vector<std::string> PartioFileReader::attributes() const {
    int num_attrs = _handle->numAttributes();
    std::vector<std::string> attr_names(num_attrs);
    for (int j = 0; j < num_attrs; ++j) {
        Partio::ParticleAttribute attr;
        _handle->attributeInfo(j, attr);
        attr_names[j] = attr.name;
    }
    return attr_names;
}


mtao::ColVecs3d PartioFileReader::positions() const {
    return vector_attribute<float, 3>("position").cast<double>();
}
mtao::ColVecs3d PartioFileReader::velocities() const {
    return vector_attribute<float, 3>("velocity").cast<double>();
}
mtao::ColVecs4d PartioFileReader::colors() const {
    return vector_attribute<float, 4>("color").cast<double>();
}
mtao::VecXi PartioFileReader::ids() const {
    return attribute<int>("id");
}
mtao::VecXd PartioFileReader::densities() const {
    return attribute<float>("density").cast<double>();
}

mtao::VecXd PartioFileReader::radii() const {
    return attribute<float>("radius").cast<double>();
}


bool PartioFileReader::has_positions() const {
    return has_attribute<float, 3>("position");
}
bool PartioFileReader::has_velocities() const {
    return has_attribute<float, 3>("velocity");
}
bool PartioFileReader::has_colors() const {
    return has_attribute<float, 3>("color");
}
bool PartioFileReader::has_ids() const {
    return has_attribute<int>("id");
}

bool PartioFileReader::has_densities() const {
    return has_attribute<float>("density");
}

bool PartioFileReader::has_radii() const {
    return has_attribute<float, 1>("radius");
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
