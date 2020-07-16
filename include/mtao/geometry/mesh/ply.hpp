#pragma once
#include <optional>
#include <ostream>
#include <vector>

namespace mtao::geometry::mesh {
class PLY {
   public:
    enum class Format { ASCII, BinaryLittleEndian, BinaryBigEndian };
    struct Element;

    Format format;
    std::vector<Element> elements;

    void write_header(std::ostream& os) const;
};

struct PLY::Element {
    struct Property;
    std::string name;
    std::vector<Property> properties;
    size_t count;
    void write_header(std::ostream& os) const;
};
struct PLY::Element::Property {
    enum class PropertyType {
        Char,
        UChar,
        Short,
        UShort,
        Int,
        UInt,
        Float,
        Double,
    };

    // the IndexType is required for template lookup purposes
    // template <PropertyType IndexType, PropertyType ValueType>
    // struct ListData {
    //    std::vector<typename Type<ValueType>> values;
    //};

    std::string name;
    PropertyType type;
    // For list types an index type is set. we use the existence of this to
    // define whether we are handling a list or not
    std::optional<PropertyType> index_type = {};
    bool is_list() const { return bool(index_type); }

    // the count is ignored if this is not a list type
    size_t byte_size(size_t count = 1) const;
    void write_header(std::ostream& os) const;
};

}  // namespace mtao::geometry::mesh
