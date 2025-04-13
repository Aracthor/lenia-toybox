#version 300 es

precision mediump float;

uniform vec2 uniResolution;
uniform sampler2D input_texture;

const int MAX_KERNELS_COUNT=3;
const int MAX_RING_COUNT=3;

struct Kernel
{
    float growthGaussCenter;
    float growthGaussWidth;
    int ringCount;
    float ringWeights[MAX_RING_COUNT];
};

uniform int uniRange;
uniform float uniDelaTime;
uniform float uniKernelGaussCenter;
uniform float uniKernelGaussWidth;
uniform Kernel uniKernel;

out vec4 frag_color;

// bell-shaped Gaussian function
float bell(float x, float m, float s)
{
    return exp(-pow((x - m) / s, 2.0) / 2.0);
}

float pixel_weight(float distance)
{
    float bDistance = distance * float(uniKernel.ringCount);
    int ringIndex = int(bDistance);
    float ringDistance = mod(bDistance, 1.0);
    float weight = uniKernel.ringWeights[ringIndex];

    return weight * bell(ringDistance, uniKernelGaussCenter, uniKernelGaussWidth);
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
            if (distance < 1.0)
            {
                float weight = pixel_weight(distance);
                sum += value * weight;
                total_weight += weight;
            }
        }
    }
    return sum / total_weight;
}

float growthFromKernel(vec2 uv)
{
    float neighbours = average_neighbours_value(uv);
    return bell(neighbours, uniKernel.growthGaussCenter, uniKernel.growthGaussWidth) * 2.0 - 1.0;
}

void main()
{
    vec2 uv = gl_FragCoord.xy / uniResolution;
    float color = texture(input_texture, uv).r;
    float growth = growthFromKernel(uv);
    color = clamp(color + uniDelaTime * growth, 0.0, 1.0);
    frag_color = vec4(color, color, color, 1.0);
}
