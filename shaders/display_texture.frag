#version 300 es

precision mediump float;

in vec4 gl_FragCoord;

uniform sampler2D display_texture;

out vec4 frag_color;

void main()
{
    vec2 resolution = vec2(800, 600);
    vec2 uv = gl_FragCoord.xy / resolution;
    frag_color = texture(display_texture, uv);
}
