#version 300 es

precision mediump float;

in vec4 gl_FragCoord;

uniform vec2 uniResolution;
uniform sampler2D input_texture;

out vec4 frag_color;

bool is_alive(vec2 uv)
{
    return texture(input_texture, uv).r > 0.5;
}

int count_alive_neighbours(vec2 uv)
{
    const vec2 surrounds[] = vec2[]
    (
        vec2(-1, -1),
        vec2( 0, -1),
        vec2( 1, -1),
        vec2( 1,  0),
        vec2( 1,  1),
        vec2( 0,  1),
        vec2(-1,  1),
        vec2(-1,  0)
    );

    int count = 0;
    for (int i = 0; i < 8; i++)
    {
        vec2 neightbour_uv = uv + surrounds[i] / uniResolution;
        count += int(is_alive(neightbour_uv));
    }
    return count;
}

void main()
{
    vec2 uv = gl_FragCoord.xy / uniResolution;
    int alive_neighbours = count_alive_neighbours(uv);
    float color;
    if (is_alive(uv))
    {
        color = (alive_neighbours == 2 || alive_neighbours == 3|| alive_neighbours == 6) ? 1.0 : 0.0;
    }
    else
    {
        color = (alive_neighbours == 3) ? 1.0 : 0.0;
    }
    frag_color = vec4(color, color, color, 1.0);
}
