#pragma once
#include "mtao/visualization/imgui/interval_scaler_settings_widget.hpp"
#include <mtao/types.hpp>

namespace mtao::visualization::imgui {
class ColorMapSettingsWidget : public IntervalScalerSettingsWidget {

    mtao::ColVecs3d colormap(const mtao::VecXd &d) const;
    enum ColorMapType : char { Parula = 0,
                               Jet = 1,
                               Waves = 2 };

    bool gui();

    ColorMapType type;
};
}// namespace mtao::visualization::imgui
