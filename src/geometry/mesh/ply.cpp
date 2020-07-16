#include "mtao/geometry/mesh/ply.hpp"

#include <fmt/ostream.h>
#include <fmt/ranges.h>

namespace mtao::geometry::mesh {

namespace internal {
template <PLY::Element::Property::PropertyType>
struct prop_type;
template <>
struct prop_type<PLY::Element::Property::PropertyType::Char> {
    using type = int8_t;
    const static std::string str;
};
template <>
struct prop_type<PLY::Element::Property::PropertyType::UChar> {
    using type = uint8_t;
    const static std::string str;
};
template <>
struct prop_type<PLY::Element::Property::PropertyType::Short> {
    using type = int16_t;
    const static std::string str;
};
template <>
struct prop_type<PLY::Element::Property::PropertyType::UShort> {
    using type = uint16_t;
    const static std::string str;
};
template <>
struct prop_type<PLY::Element::Property::PropertyType::Int> {
    using type = int32_t;
    const static std::string str;
};
template <>
struct prop_type<PLY::Element::Property::PropertyType::UInt> {
    using type = uint32_t;
    static const std::string str;
};
template <>
struct prop_type<PLY::Element::Property::PropertyType::Float> {
    using type = float;
    static const std::string str;
};
template <>
struct prop_type<PLY::Element::Property::PropertyType::Double> {
    using type = double;
    static const std::string str;
};
const std::string prop_type<PLY::Element::Property::PropertyType::Char>::str =
    "char";
const std::string prop_type<PLY::Element::Property::PropertyType::UChar>::str =
    "uchar";
const std::string prop_type<PLY::Element::Property::PropertyType::Short>::str =
    "short";
const std::string prop_type<PLY::Element::Property::PropertyType::UShort>::str =
    "ushort";
const std::string prop_type<PLY::Element::Property::PropertyType::Int>::str =
    "int";
const std::string prop_type<PLY::Element::Property::PropertyType::UInt>::str =
    "uint";
const std::string prop_type<PLY::Element::Property::PropertyType::Float>::str =
    "float";
const std::string prop_type<PLY::Element::Property::PropertyType::Double>::str =
    "ufloat";
template <PLY::Element::Property::PropertyType T>
using prop_type_t = typename prop_type<T>::type;

size_t single_element_byte_size(PLY::Element::Property::PropertyType type) {
    using PT = PLY::Element::Property::PropertyType;
    switch (type) {
#define MTAO_PT_TYPECASE(T) \
    case T:                 \
        return sizeof(prop_type_t<T>);
        MTAO_PT_TYPECASE(PT::Char)
        MTAO_PT_TYPECASE(PT::UChar)
        MTAO_PT_TYPECASE(PT::Short)
        MTAO_PT_TYPECASE(PT::UShort)
        MTAO_PT_TYPECASE(PT::Int)
        MTAO_PT_TYPECASE(PT::UInt)
        MTAO_PT_TYPECASE(PT::Float)
        MTAO_PT_TYPECASE(PT::Double)
    }
#undef MTAO_PT_TYPECASE
    return 0;
}
const std::string& property_type_name(
    PLY::Element::Property::PropertyType type) {
    using PT = PLY::Element::Property::PropertyType;
    switch (type) {
#define MTAO_PT_TYPECASE(T) \
    case T:                 \
        return prop_type<T>::str;
        MTAO_PT_TYPECASE(PT::Char)
        MTAO_PT_TYPECASE(PT::UChar)
        MTAO_PT_TYPECASE(PT::Short)
        MTAO_PT_TYPECASE(PT::UShort)
        MTAO_PT_TYPECASE(PT::Int)
        MTAO_PT_TYPECASE(PT::UInt)
        MTAO_PT_TYPECASE(PT::Float)
        MTAO_PT_TYPECASE(PT::Double)
#undef MTAO_PT_TYPECASE
    }
    const static std::string empty = "";
    return empty;
}
}  // namespace internal

void PLY::write_header(std::ostream& os) const {
    fmt::print(os, "ply\n");
    switch (format) {
        case Format::ASCII:
            fmt::print(os, "format ascii 1.0\n");
            break;
        case Format::BinaryLittleEndian:
            fmt::print(os, "format binary_little_endian 1.0\n");
            break;
        case Format::BinaryBigEndian:
            fmt::print(os, "format binary_big_endian 1.0\n");
            break;
    }
    for (auto&& element : elements) {
        element.write_header(os);
    }
    fmt::print(os, "end_header\n");
}

void PLY::Element::write_header(std::ostream& os) const {
    fmt::print(os, "element {} {}\n", name, count);
    for (auto&& property : properties) {
        property.write_header(os);
    }
}
void PLY::Element::Property::write_header(std::ostream& os) const {
    if (is_list()) {
        fmt::print(os, "property list {} {} {}\n",
                   internal::property_type_name(*index_type),
                   internal::property_type_name(*index_type), name);
    } else {
        fmt::print(os, "property {} {}\n",
                   internal::property_type_name(*index_type), name);
    }
}

// the count is ignored if this is not a list type
size_t PLY::Element::Property::byte_size(size_t count) const {
    using namespace internal;
    if (index_type) {
        return internal::single_element_byte_size(*index_type) +
               count * internal::single_element_byte_size(type);
    } else {
        return internal::single_element_byte_size(type);
    }
}
}  // namespace mtao::geometry::mesh
