#include <iostream>

#include "mtao/opengl/Window.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-in-bool-context"
#include <Eigen/Geometry>
#pragma GCC diagnostic pop
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/ArrayView.h>
#include <Magnum/Trade/AbstractImageConverter.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/PluginManager/PluginMetadata.h>
#include <Corrade/Utility/Arguments.h>
#include <Magnum/EigenIntegration/Integration.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Shaders/VertexColor.h>
#include <colormap/colormap.h>
#include <mtao/eigen/stack.h>
#include <mtao/eigen_utils.h>
#include <mtao/geometry/grid/grid.h>
#include <mtao/opengl/objects/grid.h>

#include <algorithm>
#include <memory>
#include <mtao/geometry/interpolation/radial_basis_function.hpp>
#include <mtao/opengl/shaders/vector_field.hpp>
#include <mtao/types.hpp>

#include "imgui.h"
#include "mtao/geometry/bounding_box.hpp"
#include "mtao/geometry/mesh/boundary_facets.h"
#include "mtao/geometry/mesh/read_obj.hpp"
#include "mtao/opengl/drawables.h"

using namespace Magnum::Math::Literals;

using namespace mtao::opengl;

void do_thing() {

    /*
    CORRADE_RESOURCE_INITIALIZE(PngImageConverter);
    spdlog::info("Searching for a png support in Magnum plugin dirs:: {}", fmt::join(Corrade::PluginManager::AbstractPlugin::pluginSearchPaths(), ","));
    CORRADE_PLUGIN_IMPORT(PngImageConverter);
    PluginManager::Manager<Magnum::Trade::AbstractImageConverter> manager;
    if (!(manager.load("PngImageConverter") & PluginManager::LoadState::Loaded)) {
        Utility::Error{} << "The requested plugin"
                         << "PngImageConverter"
                         << "cannot be loaded.";
    }
    Containers::Pointer<Magnum::Trade::AbstractImageConverter> image = manager.instantiate("PngImageConverter");
    */
    //return image;
}

class MeshViewer : public mtao::opengl::Window2 {
  public:
    enum class ViewMode { PointDrawing,
                          SmoothedParticleHydrodynamics };
    ViewMode view_mode = ViewMode::SmoothedParticleHydrodynamics;
    float timestep = 0.01;
    bool animate = false;
    using Vec = mtao::VectorX<GLfloat>;
    using Vec2 = mtao::Vec2f;
    Eigen::AlignedBox<float, 2> bbox;

    std::array<int, 2> N{ { 20, 20 } };
    int &NI = N[0];
    int &NJ = N[1];
    Vector2 cursor;
    mtao::Vec2f center = mtao::Vec2f(0.f, 0.f);
    mtao::ColVecs2f sph_P;
    mtao::ColVecs2f sph_V;
    mtao::VecXf sph_M;
    float desired_density = .8;
    float spring_constant = 10.0;
    float radius = 0.1f;
    float default_mass = 1.;
    int particle_count = 1000;

    int rbf_mode = 3;

    float sph_cfl() const { return radius / sph_V.colwise().norm().maxCoeff(); }

    MeshViewer(const Arguments &args) : Window2(args) {
        do_thing();
        bbox.min().setConstant(-1);
        bbox.max().setConstant(1);

        edge_drawable = new mtao::opengl::MeshDrawable<Magnum::Shaders::Flat2D>{
            grid, _flat_shader, drawables()
        };
        edge_drawable->activate_triangles({});
        edge_drawable->activate_edges();
        edge_drawable->deactivate();
        grid.setParent(&root());

        face_drawable =
          new mtao::opengl::MeshDrawable<Magnum::Shaders::VertexColor2D>{
              grid, _vcolor_shader, drawables()
          };
        face_drawable->deactivate();

        visible_grid.setParent(&scene());
        visible_drawable =
          new mtao::opengl::MeshDrawable<Magnum::Shaders::Flat2D>{
              visible_grid, _flat_shader, drawables()
          };
        visible_drawable->deactivate();
        vfield_mesh.setParent(&scene());
        _vf_viewer =
          new mtao::opengl::MeshDrawable<mtao::opengl::VectorFieldShader<2>>{
              vfield_mesh, _vf_shader, drawables()
          };
        _vf_viewer->deactivate();

        sph_particle_drawable =
          new mtao::opengl::MeshDrawable<Magnum::Shaders::Flat2D>{
              sph_mesh, _flat_shader, drawables()
          };
        sph_particle_drawable->point_size = 10;

        sph_velocity_drawable =
          new mtao::opengl::MeshDrawable<mtao::opengl::VectorFieldShader<2>>{
              sph_mesh, _vf_shader, drawables()
          };

        sph_mesh.setParent(&root());
        resample_sph(particle_count);
        update();


        set_recording_frame_callback([](int index) -> bool {
            spdlog::info("I was asked to use frame {}", index);
            return true;
        });
    }
    auto rbf(auto &&c, auto &&p) const {
        using namespace mtao::geometry::interpolation;

        switch (rbf_mode) {
        case 0:
        default:
            return HatRadialBasisFunction<float, 2>{}.evaluate(c, radius, p);
        case 1:
            return GaussianRadialBasisFunction<float, 2>{}.evaluate(
              c, radius, p);
        case 2:
            return SplineGaussianRadialBasisFunction<float, 2>{}.evaluate(
              c, radius, p);
        case 3:
            return DesbrunSplineRadialBasisFunction<float, 2>{}.evaluate(
              c, radius, p);
        }
    }
    auto rbf_grad(auto &&c, auto &&p) const {
        using namespace mtao::geometry::interpolation;

        switch (rbf_mode) {
        case 0:
        default:
            return HatRadialBasisFunction<float, 2>{}.evaluate_grad(
              c, radius, p);
        case 1:
            return GaussianRadialBasisFunction<float, 2>{}.evaluate_grad(
              c, radius, p);
        case 2:
            return SplineGaussianRadialBasisFunction<float, 2>{}
              .evaluate_grad(c, radius, p);
        case 3:
            return DesbrunSplineRadialBasisFunction<float, 2>{}
              .evaluate_grad(c, radius, p);
        }
    }
    auto rbf(auto &&p) const { return rbf(center, p); }
    auto rbf_grad(auto &&p) const { return rbf_grad(center, p); }
    float rbf() const { return rbf(Vec2(cursor.x(), cursor.y())); }
    Vec2 rbf_grad() const { return rbf_grad(Vec2(cursor.x(), cursor.y())); }
    void update() {
        if (view_mode == ViewMode::SmoothedParticleHydrodynamics) {
            if (_vf_viewer) {
                _vf_viewer->deactivate();
            }
            if (visible_drawable) {
                visible_drawable->deactivate();
            }
            sph_particle_drawable->deactivate();
            sph_particle_drawable->activate_points();
            sph_velocity_drawable->deactivate();
            sph_velocity_drawable->activate_points();

            update_sph_vis();

        } else {
            auto g = mtao::geometry::grid::Grid2f::from_bbox(
              bbox, std::array<int, 2>{ { NI, NJ } });
            grid.set(g);
            update_rbf();

            _vf_viewer->deactivate();
            _vf_viewer->activate_points();
            {
                auto g =
                  mtao::geometry::grid::Grid2f(std::array<int, 2>{ { 2, 2 } });

                // auto V = g.vertices();
                // mtao::RowVectorX<float> C = V.colwise()
                visible_grid.set(g);
                visible_drawable->deactivate();
                visible_drawable->activate_edges();
            }
        }
    }
    std::function<mtao::Vec4f(float)> get_colormap_func() const {
        return [](float value) -> mtao::Vec4f {
            colormap::MATLAB::Jet cm;
            auto c = cm.getColor(value);
            mtao::Vec4f r;
            r.x() = c.r;
            r.y() = c.g;
            r.z() = c.b;
            r.w() = 1.0;
            return r;
        };
    }

    void resample_sph(int count) {
        sph_P.resize(2, count);
        sph_V.resize(2, count);
        sph_M.resize(count);
        sph_M.setConstant(default_mass);
        float cx = bbox.center().x();

        for (int j = 0; j < count; ++j) {
            auto p = sph_P.col(j) = bbox.sample();
            if (p.x() > cx) {
                p.x() = 2 * cx - p.x();
            }
        }
        sph_V.setRandom();
        sph_V.setZero();
    }
    void update_sph_vis() {
        sph_mesh.setVertexBuffer(sph_P);
        // turn it into a tail
        sph_mesh.setVFieldBuffer((-sph_V).eval());
    }
    void sph_apply_bbox_bounce() {
        auto m = bbox.min();
        auto M = bbox.max();
        for (int j = 0; j < sph_P.cols(); ++j) {
            auto p = sph_P.col(j);
            auto v = sph_V.col(j);
            for (int k = 0; k < 2; ++k) {
                auto &p_ = p(k);
                auto &v_ = v(k);
                auto m_ = m(k);
                auto M_ = M(k);
                if (p_ < m_) {
                    p_ = m_;
                    if (v_ < 0) {
                        v_ *= -.9;
                    }
                }
                if (p_ > M_) {
                    p_ = M_;
                    if (v_ > 0) {
                        v_ *= -.9;
                    }
                }
            }
        }
    }
    mtao::VecXf sph_densities() const {
        mtao::VecXf rho(sph_P.cols());
        rho.setZero();

        for (int j = 0; j < sph_P.cols(); ++j) {
            auto pj = sph_P.col(j);
            auto &mj = sph_M(j);
            for (int k = j + 1; k < sph_P.cols(); ++k) {
                auto pk = sph_P.col(k);
                auto &mk = sph_M(k);
                float v = rbf(pj, pk);
                rho(j) += mk * v;
                rho(k) += mj * v;
            }
        }
        return rho;
    }
    mtao::ColVecs2f sph_pressure() const {
        mtao::ColVecs2f P;
        P.resizeLike(sph_P);
        P.setZero();
        mtao::VecXf rho = sph_densities();
        spdlog::info(
          "Density variation: [{},{}] desired is {}, so we get a variance of "
          "{}",
          rho.minCoeff(),
          rho.maxCoeff(),
          desired_density,
          (rho.array() - desired_density).pow(2).mean());
        for (int j = 0; j < sph_P.cols(); ++j) {
            auto pj = sph_P.col(j);
            auto Pj = P.col(j);
            float rhoj = rho(j);
            if (rhoj == 0) {
                continue;
            }
            auto &mj = sph_M(j);
            for (int k = 0; k < sph_P.cols(); ++k) {
                if (j == k) {
                    continue;
                }
                auto pk = sph_P.col(k);
                auto &mk = sph_M(k);
                auto g = rbf_grad(pk, pj);
                if (g.array().isFinite().all()) {
                    Pj += mk * g;
                }
            }
            Pj *= spring_constant * mj * std::max<float>(0, (rhoj - desired_density)) / (rhoj * rhoj);
        }

        return P;
    }

    void sph_step() {
        spdlog::info("Taking a sph timestep of size {}", timestep);
        float remaining = timestep;
        while (remaining > 0) {
            float substep = std::max<float>(1e-3, .5 * sph_cfl());
            if (substep > remaining) {
                substep = remaining;
            }
            spdlog::debug("{} remaining, taking step of {}", substep);
            sph_V.row(1).array() -= substep * .1;
            for (int j = 0; j < 20; ++j) {
                auto pressure = sph_pressure();
                sph_P -= substep * substep * pressure;
                sph_V -= substep / 20 * pressure;
                sph_apply_bbox_bounce();
            }
            sph_V -= substep * sph_pressure();
            sph_P += substep * sph_V;

            sph_apply_bbox_bounce();
            remaining -= substep;
        }
    }

    void update_rbf() {
        // mtao::geometry::grid::Grid2f
        // g(std::array<int,2>{{NI,NJ,NK}});
        auto g = mtao::geometry::grid::Grid2f::from_bbox(
          bbox, std::array<int, 2>{ { NI, NJ } });

        auto colormap_func = get_colormap_func();

        // auto V = g.vertices();
        auto V = g.vertices();
        mtao::ColVecs2f vel = g.vertices();
        double sum = 0;
        mtao::ColVecs4f C(4, V.cols());
        for (int j = 0; j < V.cols(); ++j) {
            auto v = V.col(j);
            float val = rbf(v);
            sum += val;
            auto c = C.col(j);
            c = colormap_func(val);
            vel.col(j) = rbf_grad(v);
        }
        sum /= V.cols();
        sum *= bbox.volume();
        std::cout << "Integrated volume: " << sum << std::endl;
        grid.setColorBuffer(C);
        vfield_mesh.setVertexBuffer(V);
        vfield_mesh.setVFieldBuffer(vel);
    }
    void do_animation() { sph_step(); }
    float scale = 1.0;
    void gui() override {
        if (ImGui::InputInt2("N", &NI)) {
            update();
        }
        if (ImGui::InputInt("RBFMode", &rbf_mode)) {
            update_rbf();
        }
        if (ImGui::SliderFloat2("min", bbox.min().data(), -2, 2)) {
            bbox.min() = (bbox.min().array() < bbox.max().array())
                           .select(bbox.min(), bbox.max());
            update();
        }
        if (ImGui::SliderFloat2("max", bbox.max().data(), -2, 2)) {
            bbox.max() = (bbox.min().array() > bbox.max().array())
                           .select(bbox.min(), bbox.max());
            update();
        }
        if (ImGui::SliderFloat("Radius", &radius, 0, 1)) {
            update();
        }
        if (_vf_viewer) {
            _vf_viewer->gui();
        }
        if (edge_drawable) {
            edge_drawable->gui();
        }
        if (face_drawable) {
            face_drawable->gui();
        }
        if (sph_particle_drawable) {
            sph_particle_drawable->gui("SPH Particle");
        }
        if (sph_velocity_drawable) {
            sph_velocity_drawable->gui("SPH Velocity");
        }
        if (ImGui::Button("Set desired density to current")) {
            desired_density = sph_densities().mean();
        }
        if (ImGui::InputFloat("Desired density", &desired_density)) {
        }
        if (ImGui::InputFloat("default mass", &default_mass)) {
        }
        if (ImGui::InputFloat("spring constant", &spring_constant)) {
        }
        if (ImGui::InputInt("SPH Particle count", &particle_count)) {
            //resample_sph(particle_count);
        }
        if (ImGui::Button("Resample")) {
            resample_sph(particle_count);
            update();
        }
        ImGui::Checkbox("animate", &animate);
        if (ImGui::Button("Step")) {
            do_animation();
            update();
        }
        WindowBase::recording_gui();
        ImGui::Text("Cursor Position: (%f,%f) = %f (d=%f)", cursor.x(), cursor.y(), rbf(), (mtao::Vec2f(cursor.x(), cursor.y()) - center).norm());
    }
    void draw() override {
        if (animate) {
            do_animation();
            update();
        }
        Magnum::GL::Renderer::disable(
          Magnum::GL::Renderer::Feature::FaceCulling);
        Magnum::GL::Renderer::setPointSize(10.);
        Window2::draw();
    }

    void mouseMoveEvent(MouseMoveEvent &event) override {
        Window2::mouseMoveEvent(event);
        cursor = localPosition(event.position());
    }
    void mousePressEvent(MouseEvent &event) override {
        Window2::mousePressEvent(event);
        if (!ImGui::GetIO().WantCaptureMouse) {
            if (event.button() == MouseEvent::Button::Left) {
                center = EigenIntegration::cast<mtao::Vec2f>(cursor);
                update_rbf();
            }
        }
    }

  private:
    Magnum::Shaders::Flat2D _flat_shader;
    Magnum::Shaders::VertexColor2D _vcolor_shader;
    mtao::opengl::VectorFieldShader<2> _vf_shader;
    mtao::opengl::objects::Grid<2> grid;
    mtao::opengl::objects::Grid<2> visible_grid;
    mtao::opengl::objects::Mesh<2> vfield_mesh;

    mtao::opengl::objects::Mesh<2> sph_mesh;
    mtao::opengl::MeshDrawable<Magnum::Shaders::Flat2D> *edge_drawable =
      nullptr;
    mtao::opengl::MeshDrawable<Magnum::Shaders::VertexColor2D> *face_drawable =
      nullptr;
    mtao::opengl::MeshDrawable<Magnum::Shaders::Flat2D> *visible_drawable =
      nullptr;
    mtao::opengl::MeshDrawable<mtao::opengl::VectorFieldShader<2>> *_vf_viewer =
      nullptr;

    mtao::opengl::MeshDrawable<Magnum::Shaders::Flat2D> *sph_particle_drawable =
      nullptr;
    mtao::opengl::MeshDrawable<mtao::opengl::VectorFieldShader<2>> *
      sph_velocity_drawable = nullptr;
};

MAGNUM_APPLICATION_MAIN(MeshViewer)
