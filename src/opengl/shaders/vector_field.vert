#if defined(PER_VERTEX_COLOR)
in lowp vec4 color;
#else
uniform lowp vec4 color = vec4(1,0,0,1);
#endif
uniform highp float scale = 1;

#if defined(TWO_DIMENSIONS)
uniform highp mat3 transformationProjectionMatrix;
layout(location = 0)
in highp vec2 position;
layout(location = 5)
in highp vec2 direction;
#else
uniform highp mat4 transformationProjectionMatrix;
layout(location = 0)
in highp vec4 position;
layout(location = 5)
in highp vec3 direction;
#endif


out lowp vec4 head_position;
out lowp vec4 geomColor;

void main() {


#if defined(TWO_DIMENSIONS)
    gl_Position.xywz = vec4(transformationProjectionMatrix * vec3(position,1),0);
    head_position.xywz = vec4(transformationProjectionMatrix * vec3(position + scale * direction,1),0);
#else
    gl_Position = transformationProjectionMatrix * position;
    head_position = transformationProjectionMatrix * (position + vec4( scale * direction,0));
#endif
    geomColor = color;
}
