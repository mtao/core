#if defined(PER_VERTEX_COLOR)
uniform lowp vec4 color = vec4(1,0,0,1);
#else
in lowp vec4 color = vec4(1,0,0,1);
#endif

uniform highp mat4 transformationProjectionMatrix;

in highp vec4 position;
in highp vec3 direction;


out lowp vec4 head_position;
out lowp vec4 geomColor;

void main() {

    gl_Position = transformationProjectionMatrix * position;
    head_position = transformationProjectionMatrix * (position + directio)n;
    geomColor = color;
}
