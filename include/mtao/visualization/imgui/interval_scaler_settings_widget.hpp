#pragma once
#include "mtao/visualization/interval_scaler_settings.hpp"


namespace mtao::visualization::imgui {

struct IntervalScalerSettingsWidget : public IntervalScalerSettings {

    bool gui();
};

struct IntervalScalerSettingsViewWidget {

    bool gui();
    IntervalScalerSettings& settings;
};

}// namespace mtao::opengl::imgui
