#version 150
in vec2 coords;
out vec2 my_pos;

void main() {
    float ratio = 1.33333;
    mat4 scale = mat4(vec4(0.5 / ratio, 0.0, 0.0, 0.0), vec4(0.0, 0.5, 0.0, 0.0), vec4(0.0, 0.0, 1.0, 0.0), vec4(0.0, 0.0, 0.0, 1.0));
    my_pos = coords;
    gl_Position = scale * vec4(coords, 0.0, 1.0);
}
