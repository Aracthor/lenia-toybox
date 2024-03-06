#version 300 es

precision mediump float;

uniform vec2 uniResolution;
uniform sampler2D input_texture;

uniform int uniRange;
uniform float uniDelaTime;
uniform vec2 uniSurvival;
uniform vec2 uniBirth;

out vec4 frag_color;

float average_neighbour_value(vec2 uv)
{
    float total = 0.0;
    int neighbour_count = 0;
    for (int y = -uniRange; y <= uniRange; y++)
    {
        for (int x = -uniRange; x <= uniRange; x++)
        {
            if (x != 0 || y != 0)
            {
                vec2 neightbour_uv = uv + vec2(x, y) / uniResolution;
                total += texture(input_texture, neightbour_uv).r;
                neighbour_count += 1;
            }
        }
    }
    return total / float(neighbour_count);
}

void main()
{
    vec2 uv = gl_FragCoord.xy / uniResolution;
    float average_value = average_neighbour_value(uv);
    float color = texture(input_texture, uv).r;
    if (average_value >= uniBirth.x && average_value <= uniBirth.y)
    {
        color += uniDelaTime;
    }
    else if (average_value < uniSurvival.x || average_value > uniSurvival.y)
    {
        color -= uniDelaTime;
    }
    color = clamp(color, 0.0, 1.0);
    frag_color = vec4(color, color, color, 1.0);
}
