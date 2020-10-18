#if defined(TWO_DIMENSIONS)
in vec2 vertPos;
#else
in vec3 vertPos;
#endif
out lowp vec4 outFragColor;

uniform highp float colormap_scale = 1.0;
uniform highp float colormap_shift = 0.0;


void main() {
    highp float val = polynomial_eval(vertPos);
    highp float m = mod(val,1);
    if(m < .01 || m > .99) {
        outFragColor = vec4(1);
    } else {
    val = val * colormap_scale + colormap_shift;
    val = (val + 1) / 2;
    outFragColor = colormap(val);
    }
}
