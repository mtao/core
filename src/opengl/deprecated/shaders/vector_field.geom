layout(points) in;
in vec4 geomColor[1];
in vec4 head_position[1];

layout(line_strip, max_vertices = 2) out;

out lowp vec4 fragColor;


void main() {
    gl_Position = gl_in[0].gl_Position;
    fragColor = vec4(1);
    EmitVertex();
    gl_Position = head_position[0];
    fragColor = geomColor[0];
    EmitVertex();
}
