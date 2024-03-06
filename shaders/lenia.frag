#version 300 es

precision mediump float;

uniform vec2 uniResolution;
uniform sampler2D input_texture;

uniform int uniRange;
uniform float uniDelaTime;

out vec4 frag_color;

// bell-shaped Gaussian function
float bell(float x, float m, float s)
{
    return exp(-pow((x - m) / s, 2.0) / 2.0);
}

float average_neighbours_value(vec2 uv)
{
    float sum = 0.0;
    float total_weight = 0.0;
    for (int y = -uniRange; y <= uniRange; y++)
    {
        for (int x = -uniRange; x <= uniRange; x++)
        {
            vec2 neightbour_uv = uv + vec2(x, y) / uniResolution;
            float value = texture(input_texture, neightbour_uv).r;
            float distance = sqrt(float(x * x + y * y)) / float(uniRange);
            float weight = bell(distance, 0.5, 0.15);
            sum += value * weight;
            total_weight += weight;
        }
    }
    return sum / total_weight;
}

void main()
{
    vec2 uv = gl_FragCoord.xy / uniResolution;
    float color = texture(input_texture, uv).r;
    float neighbours = average_neighbours_value(uv);
    const float m = 0.135;
    const float s = 0.014;
    float growth = bell(neighbours, m, s) * 2.0 - 1.0;
    color = clamp(color + uniDelaTime * growth, 0.0, 1.0);
    frag_color = vec4(color, color, color, 1.0);
}
