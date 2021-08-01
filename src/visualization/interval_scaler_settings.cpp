#include "mtao/visualization/interval_scaler_settings.hpp"


namespace mtao::visualization {

void IntervalScalerSettings::update_minmax() {
    // [-1,1] = scale * [min,max] + shift
    // [-1-shift,1-shift] = scale * [min,max]
    //
    min_value = (-1 - shift) / scale;
    max_value = (1 - shift) / scale;
}

void IntervalScalerSettings::update_scale_shift() {
    // [-1,1] = scale * [min,max] + shift
    // [-1-shift,1-shift] = scale * [min,max]
    // -(1+scale)/shift = min
    // (1-scale)/shift = max

    scale = 2 / (max_value - min_value);
    shift = (min_value + max_value) / (min_value - max_value);
}
}// namespace mtao
