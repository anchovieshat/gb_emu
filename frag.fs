#version 150

in vec2 my_pos;
out vec4 frag_color;

void main() {
    frag_color = vec4(my_pos, 0.0, 1.0);
}
