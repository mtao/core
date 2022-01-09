#include <Partio.h>
#include <PartioAttribute.h>
#include "mtao/geometry/point_cloud/partio_loader.hpp"
#include <spdlog/spdlog.h>
#include <fmt/format.h>

namespace mtao::geometry::point_cloud

{

template<typename T>
constexpr Partio::ParticleAttributeType
  type_to_partio_scalar() {
    if constexpr (std::is_same_v<T, int>) {
        return Partio::ParticleAttributeType::INT;
    } else if constexpr (std::is_same_v<T, float>) {
        return Partio::ParticleAttributeType::FLOAT;
    }
    return Partio::ParticleAttributeType::NONE;
}
template<eigen::concepts::ColVecsDCompatible T>
void PartioFileWriter::set_attribute(const std::string &name, const T &V) {
    using Scalar = typename T::Scalar;
    constexpr static int D = T::RowsAtCompileTime;

    Partio::ParticleAttributeType type = type_to_partio_scalar<Scalar>();

    if constexpr (D == 3 && std::is_same_v<Scalar, float>) {
        type = Partio::ParticleAttributeType::VECTOR;
    }
    Partio::ParticleAttribute attr = _handle->addAttribute(
      name.c_str(), type, D);
    update_size(V.cols());
    auto it = _handle->begin();
    for (int j = 0; j < V.cols(); ++j, ++it) {
        auto p = V.col(j);
        float *dat = _handle->dataWrite<float>(attr, it.index);
        Eigen::Map<mtao::Vector<Scalar, D>>{ dat } = p;
    }
}

template<eigen::concepts::VecXCompatible T>
void PartioFileWriter::set_attribute(const std::string &name, const T &V) {

    using Scalar = typename T::Scalar;
    Partio::ParticleAttributeType type = type_to_partio_scalar<Scalar>();
    Partio::ParticleAttribute attr = _handle->addAttribute(
      name.c_str(), type, 1);

    update_size(V.size());

    auto it = _handle->begin();
    for (int j = 0; j < V.size(); ++j, ++it) {
        Scalar *dat = _handle->dataWrite<Scalar>(attr, it.index);
        *dat = V(j);
    }
}

template<typename T, int D>
bool PartioFileReader::has_attribute(const std::string &name) const {
    Partio::ParticleAttribute attr;
    if (_handle->attributeInfo(name.c_str(), attr) && Partio::typeCheck<T>(attr.type) && attr.count == D) {
        return true;
    }
    return false;
}

template<typename T, int D>
mtao::ColVectors<T, D> PartioFileReader::vector_attribute(const std::string &name) const {

    if (!has_attribute<T, D>(name)) {
        throw std::invalid_argument(fmt::format("failed to get {} as a vector of size {}", name, D));
    }
    Partio::ParticleAttribute attr;
    _handle->attributeInfo(name.c_str(), attr);

    mtao::ColVectors<T, D> R(D, particle_count());

    auto iterator = _handle->begin();
    Partio::ParticleAccessor acc(attr);
    iterator.addAccessor(acc);

    int cnt = 0;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wambiguous-reversed-operator"
    for (auto it = _handle->begin(); it != _handle->end(); ++it) {
#pragma GCC diagnostic pop
        float *data = acc.raw<T>(it);
        R.col(cnt++) = Eigen::Map<const mtao::Vector<T, D>>{ data };
    }
    return R;
}
template<typename T>
mtao::VectorX<T> PartioFileReader::attribute(const std::string &name) const {

    if (!has_attribute<T, 1>(name)) {
        throw std::invalid_argument(fmt::format("failed to get {} as a vector of size {}", name, 1));
    }
    Partio::ParticleAttribute attr;
    _handle->attributeInfo(name.c_str(), attr);

    mtao::VectorX<T> R(particle_count());
    auto iterator = _handle->begin();
    Partio::ParticleAccessor acc(attr);
    iterator.addAccessor(acc);

    int cnt = 0;
    for (auto it = _handle->begin(); it != _handle->end(); ++it) {
        T *data = acc.raw<T>(it);
        R(cnt++) = *data;
    }
    return R;
}

}// namespace mtao::geometry::point_cloud
