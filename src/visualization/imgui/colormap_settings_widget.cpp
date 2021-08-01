#include "mtao/visualization/imgui/colormap_settings_widget.hpp"
#include <imgui.h>
#include <colormap/colormap.h>


namespace mtao::visualization::imgui {


bool ColorMapSettingsWidget::gui() {
    bool ret = false;

    {
        static const char *items[] = {
            "Parula",
            "Jet",
            "Waves"
        };
        int m = static_cast<char>(type);
        ret |= ImGui::Combo("Type", &m, items, IM_ARRAYSIZE(items));
        type = static_cast<ColorMapType>(char(m));
    }

    ret |= IntervalScalerSettingsWidget::gui();
    return ret;
}
mtao::ColVecs4d ColorMapSettingsWidget::get_rgba(const mtao::VecXd &D) const {
    return get_rgba_T<double>(D);
}
mtao::ColVecs4f ColorMapSettingsWidget::get_rgba(const mtao::VecXf &D) const {
    return get_rgba_T<float>(D);
}

template<typename Scalar>
mtao::ColVectors<Scalar, 4> ColorMapSettingsWidget::get_rgba_T(const mtao::VectorX<Scalar> &D) const {

    mtao::ColVectors<Scalar, 4> R(4, D.size());
    for (int j = 0; j < D.size(); ++j) {
        R.col(j) = get_color(D(j)).template cast<Scalar>();
    }
    return R;
}

mtao::Vec4d ColorMapSettingsWidget::get_color(double x) const {
    x = scale * x + shift;
    colormap::Color c;
    switch (type) {
    default:
    case ColorMapType::Parula: {
        c = colormap::MATLAB::Parula().getColor(x);
        break;
    }
    case ColorMapType::Jet: {
        c = colormap::MATLAB::Jet().getColor(x);
        break;
    }
    case ColorMapType::Waves: {
        c = colormap::transform::LavaWaves().getColor(x);
        break;
    }
    }
    return mtao::Vec4d(c.r, c.g, c.b, c.a);
}


}// namespace mtao::visualization::imgui
