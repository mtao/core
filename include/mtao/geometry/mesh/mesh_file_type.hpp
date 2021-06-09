#pragma once
#include <string_view>

namespace mtao::geometry::mesh {
    // our sad set of mesh files we can write to
    enum class MeshFileType {
        PLY,OBJ
    };

    MeshFileType get_mesh_file_type(const std::string_view& filename);

}
