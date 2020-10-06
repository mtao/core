#if defined(TWO_DIMENSIONS)
uniform highp mat3 transformationProjectionMatrix;
layout(location = 0)
in highp vec2 position;
#else
uniform highp mat4 transformationProjectionMatrix;
layout(location = 0)
in highp vec3 position;
#endif


#if defined(TWO_DIMENSIONS)
out vec2 vertPos;
#else
out vec3 vertPos;
#endif

void main() {


#if defined(TWO_DIMENSIONS)
    gl_Position.xywz = vec4(transformationProjectionMatrix * vec3(position,1),0);
#else
    gl_Position = transformationProjectionMatrix * position;
#endif
    vertPos = position;
}
