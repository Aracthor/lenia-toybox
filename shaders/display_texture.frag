#version 300 es

precision mediump float;

uniform vec2 uniResolution;
uniform sampler2D display_texture;

out vec4 frag_color;

void main()
{
    vec2 uv = gl_FragCoord.xy / uniResolution;
    float color = texture(display_texture, uv).r;
    frag_color = vec4(color, color, color, 1.0);
}
