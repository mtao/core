#include <iostream>
#include <mtao/geometry/mesh/read_obj.hpp>
#include <mtao/iterator/enumerate.hpp>
#include <cxxopts.hpp>
#include <tbb/parallel_for.h>
#include <filesystem>
#include <fstream>
#include <spdlog/spdlog.h>
#include <fmt/format.h>
#include "partio_sequence_tools/particles.hpp"
#include "partio_sequence_tools/filter.hpp"
#include <nlohmann/json.hpp>


class ParticleData {
  public:
    ParticleData(const std::filesystem::path &path);
    std::filesystem::path mesh_path() const;

    std::string frame_fmt() const;
    std::filesystem::path frame_path(int index) const;
    Particles frame_particles_from_disk(int index) const;

    bool frame_exists(int index) const;
    int frame_file_count() const;
    void save_filtered_particle(const std::string &path_format, size_t index, const std::vector<int> &indices);

    std::string relative_format = "frame_{}/particles.bgeo.gz";

    std::filesystem::path base_dir;
    std::vector<Particles> particles;
};


ParticleData::ParticleData(const std::filesystem::path &path) : base_dir(path) {

    int num_frames = frame_file_count();
    particles.resize(num_frames);

    tbb::parallel_for(0, num_frames, [&](int j) {
        particles[j] = frame_particles_from_disk(j);
    });
    spdlog::info("Loaded ParticleData from [{}] with {} particles", std::string(path), num_frames);
}

void ParticleData::save_filtered_particle(const std::string &path_fmt, size_t index, const std::vector<int> &indices) {

    auto parent_path = std::filesystem::path(path_fmt).parent_path();
    if (std::filesystem::exists(parent_path)) {
        if (!std::filesystem::is_directory(parent_path)) {
            spdlog::error("Cannot save filtered particles because [{}] is not a directory", std::string(parent_path));
        }
    } else {
        bool created_parent = std::filesystem::create_directories(parent_path);
        if (created_parent) {
            spdlog::info("Created directory to save particles {}", std::string(parent_path));
        }
    }


    std::filesystem::path path = fmt::vformat(path_fmt, fmt::make_format_args(index));

    const auto &parts = particles[index];
    if (path.extension() == ".obj") {
        parts.save_subset_obj(path, indices);
    } else {
        parts.save_subset_obj(path, indices);
    }
}

std::string ParticleData::frame_fmt() const {
    return base_dir / relative_format;
}
std::filesystem::path ParticleData::frame_path(int index) const {
    return fmt::vformat(frame_fmt(), fmt::make_format_args(index));
}
Particles ParticleData::frame_particles_from_disk(int index) const {
    return Particles(frame_path(index));
}

std::filesystem::path ParticleData::mesh_path() const {
    std::filesystem::path local_path = base_dir / "mesh.obj";
    if (std::filesystem::exists(local_path)) {
        return local_path;
    }

    std::filesystem::path meshinfo_path = base_dir / "mesh_info.json";
    if (std::filesystem::exists(meshinfo_path)) {
        nlohmann::json js;
        std::ifstream(meshinfo_path) >> js;

        local_path = js["mesh"]["filename"].get<std::string>();
        if (std::filesystem::exists(local_path)) {
            return local_path;
        }
    }
    spdlog::warn("No valid mesh file path found");
    return {};
}
bool ParticleData::frame_exists(int index) const {
    auto path = frame_path(index);
    bool res = std::filesystem::exists(path);
    return res;
}
int ParticleData::frame_file_count() const {
    int j;
    for (j = 0; frame_exists(j); ++j)
        ;
    return j;
}


int main(int argc, char *argv[]) {

    cxxopts::Options opts("mesh_view", "a simple mesh statistics tool");

    // clang-format off
    opts.add_options()
        ("input_path", "Input path",cxxopts::value<std::string>())
        ("output_path", "Output path",cxxopts::value<std::string>())
        ("m,mesh_distance", "offset distance from mesh", cxxopts::value<double>()->default_value("0.1"))
        ("d,dense", "filter for only dense particles", cxxopts::value<bool>()->default_value("true"))
        ("h,help", "Print usage");

    opts.parse_positional({"input_path","output_path"});

    auto res = opts.parse(argc, argv);

    std::string input_path = res["input_path"].as<std::string>();
    std::string output_path = res["output_path"].as<std::string>();
    double mesh_distance = res["mesh_distance"].as<double>();
    bool dense = res["dense"].as<bool>();

    ParticleData pd(input_path);


    std::shared_ptr<MeshFilter> mesh_filter = [&]() -> std::shared_ptr<MeshFilter> {
        auto p = pd.mesh_path();
        if (std::filesystem::exists(p)) {
            auto [V, F] = mtao::geometry::mesh::read_objF(p);
            V.row(1) *= -1;
            V.row(2) *= -1;
            return std::make_shared<MeshFilter>(V.transpose(), F.transpose());
        } else {
            spdlog::error("Mesh file [{}] not found", std::string(p));
                return {};
        }
    }();
    if(!bool(mesh_filter))
    {
        return 1;
    }
    auto tr = [](double v) -> double {
        return std::numbers::pi_v<double> * v / 180.0;
    };
    mesh_filter->point_transform.rotate(Eigen::AngleAxisd(tr(180), mtao::Vec3d::UnitX()));
    mesh_filter->point_transform.rotate(Eigen::AngleAxisd(tr(0), mtao::Vec3d::UnitY()));
    mesh_filter->point_transform.rotate(Eigen::AngleAxisd(tr(0), mtao::Vec3d::UnitZ()));
    mesh_filter->point_transform =
        mesh_filter->point_transform.inverse();

    auto range_filter = std::make_shared<RangeFilter>();
    range_filter->_range_mode = RangeFilter::RangeMode::Density;
    range_filter->range = std::array<float,2>{{0.5,1.0}};

    for (auto &&[index, p] : mtao::iterator::enumerate(pd.particles)) {

        auto M = mesh_filter->particle_mask(p);
        auto D = range_filter->particle_mask(p);
        spdlog::info("{} dense particles {} particles close to boundary", D.count(), M.count());

        Filter::BoolVec C = D;
        //Filter::BoolVec C = M && D;

        std::vector<int> indices;
        for (auto &&[idx, val] : mtao::iterator::enumerate(C)) {
            if (val) {
                indices.emplace_back(idx);
            }
        }



        pd.save_filtered_particle(output_path, index, indices);

    }


}
