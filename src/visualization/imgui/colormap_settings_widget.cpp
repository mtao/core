#include "mtao/visualization/imgui/colormap_settings_widget.hpp"
#include <imgui.h>


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
        ImGui::Combo("Type", &m, items, IM_ARRAYSIZE(items));
        type = static_cast<ColorMapType>(char(m));
    }

    if (ImGui::InputFloat("Type", &scale)) {
        ret = true;
        update_minmax();
    }
    if (ImGui::InputFloat("Shift", &shift)) {
        ret = true;
        update_minmax();
    }
    if (ImGui::InputFloat("Colormap min", &min_value)) {
        ret = true;
        update_scale_shift();
    }
    if (ImGui::InputFloat("Colormap max", &max_value)) {
        ret = true;
        update_scale_shift();
    }
    return ret;
}


}// namespace mtao::visualization::imgui
