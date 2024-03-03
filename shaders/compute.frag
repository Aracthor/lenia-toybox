#version 300 es

precision mediump float;

in vec4 gl_FragCoord;

uniform vec2 uniResolution;
uniform sampler2D display_texture;

out vec4 frag_color;

void main()
{
    vec2 uv = gl_FragCoord.xy / uniResolution;
    float color = texture(display_texture, uv).r * 0.99;
    frag_color = vec4(color, color, color, 1.0);
}
