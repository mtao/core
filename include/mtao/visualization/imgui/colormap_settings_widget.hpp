#pragma once
#include "mtao/visualization/imgui/interval_scaler_settings_widget.hpp"
#include <mtao/types.hpp>

namespace mtao::visualization::imgui {
struct ColorMapSettingsWidget : public IntervalScalerSettingsWidget {

    mtao::ColVecs3d colormap(const mtao::VecXd &d) const;
    enum ColorMapType : char { Parula = 0,
                               Jet = 1,
                               Waves = 2,
                               Winter = 3 };

    bool gui();


    mtao::ColVecs4d get_rgba(const mtao::VecXd &) const;
    mtao::ColVecs4f get_rgba(const mtao::VecXf &) const;

  private:
    ColorMapType type = ColorMapType::Parula;


    template<typename Scalar>
    mtao::ColVectors<Scalar, 4> get_rgba_T(const mtao::VectorX<Scalar> &) const;

    mtao::Vec4d get_color(double x) const;
};
}// namespace mtao::visualization::imgui
