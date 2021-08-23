#include "mtao/visualization/imgui/colormap_settings_widget.hpp"
#include "mtao/visualization/imgui/utils.hpp"

#include <imgui.h>
#include <colormap/colormap.h>


namespace mtao::visualization::imgui {

const std::array<std::string, int(ColorMapSettingsWidget::ColorMapType::END)> ColorMapSettingsWidget::ColorMapNames = {
    "Parula",
    "Jet",
    "Waves",
    "Winter"
};

bool ColorMapSettingsWidget::gui() {
    bool ret = false;

    {

        static const auto items = utils::strs_to_charPtr(ColorMapNames);
        int m = static_cast<char>(type);
        ret |= ImGui::Combo("Type", &m, items.data(), items.size());
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
    case ColorMapType::Winter: {
        c = colormap::MATLAB::Winter().getColor(x);
        break;
    }
    }
    return mtao::Vec4d(c.r, c.g, c.b, c.a);
}
void ColorMapSettingsWidget::set_type(ColorMapType type) {
    this->type = type;
}

auto ColorMapSettingsWidget::get_type() const -> ColorMapType { return type; }


}// namespace mtao::visualization::imgui
