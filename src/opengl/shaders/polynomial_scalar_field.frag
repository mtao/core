#if defined(TWO_DIMENSIONS)
in vec2 vertPos;

#else
in vec3 vertPos;
#endif
out lowp vec4 outFragColor;

uniform float colormap_scale = 1.0f;
uniform float colormap_shift = 0.0f;


void main() {
    float val = polynomial_eval(vertPos) * colormap_scale + colormap_shift;
    outFragColor = colormap((val + 1) / 2);
}
