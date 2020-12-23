#if defined(TWO_DIMENSIONS)
in vec2 vertPos;
#else
in vec3 vertPos;
#endif
out lowp vec4 outFragColor;

uniform highp float colormap_scale = 1.0;
uniform highp float colormap_shift = 0.0;

#if defined(POLY_ISOCONTOURS)
// the width of the isolines created
uniform highp float iso_line_width = 0.05f;
// distance between isovalues
uniform highp float iso_distance = 1.0f;
#endif


void main() {
    highp float val = polynomial_eval(vertPos);
#if defined(POLY_ISOCONTOURS)
    highp float m = mod(val,iso_distance);
    if(m < iso_line_width / 2 || m + iso_line_width / 2 > 1) {
        outFragColor = vec4(1);
        return;
    }
#endif
    val = val * colormap_scale + colormap_shift;
    val = (val + 1) / 2;
    outFragColor = colormap(val);
}
