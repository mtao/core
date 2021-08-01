#include "mtao/visualization/imgui/interval_scaler_settings_widget.hpp"
#include <imgui.h>


namespace mtao::visualization::imgui {


bool IntervalScalerSettingsViewWidget::gui() {
    bool ret = false;
    ;
    if (ImGui::InputFloat("Scale", &settings.scale)) {
        ret = true;
        settings.update_minmax();
    }
    if (ImGui::InputFloat("Shift", &settings.shift)) {
        ret = true;
        settings.update_minmax();
    }
    if (ImGui::InputFloat("Colormap min", &settings.min_value)) {
        ret = true;
        settings.update_scale_shift();
    }
    if (ImGui::InputFloat("Colormap max", &settings.max_value)) {
        ret = true;
        settings.update_scale_shift();
    }
    return ret;
}
bool IntervalScalerSettingsWidget::gui() {
    bool ret = false;
    if (ImGui::InputFloat("Scale", &scale)) {
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
