#version 300 es

precision mediump float;

in vec4 gl_FragCoord;

const vec2 resolution = vec2(800, 600);
uniform sampler2D display_texture;

out vec4 frag_color;

void main()
{
    vec2 uv = gl_FragCoord.xy / resolution;
    float color = texture(display_texture, uv).r;
    frag_color = vec4(color, color, color, 1.0);
}
