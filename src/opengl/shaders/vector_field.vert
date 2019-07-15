#if defined(PER_VERTEX_COLOR)
in lowp vec4 color;
#else
uniform lowp vec4 color = vec4(1,0,0,1);
#endif

uniform highp mat4 transformationMatrix;
uniform highp mat4 projectionMatrix;

in highp vec4 position;
in highp vec3 direction;


out lowp vec4 head_position;
out lowp vec4 geomColor;

void main() {

    mat4 transformationProjectionMatrix = projectionMatrix * transformationMatrix;
    gl_Position = transformationProjectionMatrix * position;
    head_position = transformationProjectionMatrix * (position + vec4(direction,0));
    geomColor = color;
}
