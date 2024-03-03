#version 300 es

precision mediump float;

in vec4 gl_FragCoord;

uniform vec2 uniResolution;
uniform sampler2D input_texture;

uniform int uniRange;
uniform ivec2 uniSurvival;
uniform ivec2 uniBirth;

out vec4 frag_color;

bool is_alive(vec2 uv)
{
    return texture(input_texture, uv).r > 0.5;
}

int count_alive_neighbours(vec2 uv)
{
    int count = 0;
    for (int y = -uniRange; y <= uniRange; y++)
    {
        for (int x = -uniRange; x <= uniRange; x++)
        {
            if (x != 0 || y != 0)
            {
                vec2 neightbour_uv = uv + vec2(x, y) / uniResolution;
                count += int(is_alive(neightbour_uv));
            }
        }
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
        color = (alive_neighbours >= uniSurvival.x && alive_neighbours <= uniSurvival.y) ? 1.0 : 0.0;
    }
    else
    {
        color = (alive_neighbours >= uniBirth.x && alive_neighbours <= uniBirth.y) ? 1.0 : 0.0;
    }
    frag_color = vec4(color, color, color, 1.0);
}
