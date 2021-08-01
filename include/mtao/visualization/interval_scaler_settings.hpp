namespace mtao::visualization {
struct IntervalScalerSettings {
    // for debug purposes one should change min_value/max_value with
    // update_minmax() whenever colormap_scale / colormap_shift are changed
    float scale = 1;
    float shift = 0;

    // convenience representation, should always call update_scale_shift() any
    // time these values are modified to activate the changes
    float min_value = -1;
    float max_value = 1;

    void update_minmax();
    void update_scale_shift();
};
}// namespace mtao::visualization
