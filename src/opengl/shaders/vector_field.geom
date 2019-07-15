layout(points) in;

layout(lines, max_vertices = 2) out;

out lowp vec4 fragColor;


int main() {
    gl_Position = gl_in[0].gl_Position;
#if defined(PER_VERTEX_COLOR)
    fragColor = vec4(1);
#else
    fragColor = gl_in[0].color;
#endif
    EmitVertex();
    gl_Position = gl_in[0].gl_Position;
    fragColor = gl_in[1].color;
    EmitVertex();
}
