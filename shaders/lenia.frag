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
uniform int uniKernelCount;
uniform Kernel uniKernels[3];

out vec4 frag_color;

// bell-shaped Gaussian function
float bell(float x, float m, float s)
{
    return exp(-pow((x - m) / s, 2.0) / 2.0);
}

float pixel_weight(int kernelIndex, float distance)
{
    float bDistance = distance * float(uniKernels[kernelIndex].ringCount);
    int ringIndex = int(bDistance);
    float ringDistance = mod(bDistance, 1.0);
    float weight = uniKernels[kernelIndex].ringWeights[ringIndex];

    return weight * bell(ringDistance, uniKernelGaussCenter, uniKernelGaussWidth);
}

float average_neighbours_value(int kernel_index, vec2 uv)
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
                float weight = pixel_weight(kernel_index, distance);
                sum += value * weight;
                total_weight += weight;
            }
        }
    }
    return sum / total_weight;
}

float growthFromKernels(vec2 uv)
{
    float growth = 0.0;
    for (int k = 0; k < uniKernelCount; k++)
    {
        float neighbours = average_neighbours_value(k, uv);
        growth += bell(neighbours, uniKernels[k].growthGaussCenter, uniKernels[k].growthGaussWidth) * 2.0 - 1.0;
    }
    return growth / float(uniKernelCount);
}

void main()
{
    vec2 uv = gl_FragCoord.xy / uniResolution;
    float color = texture(input_texture, uv).r;
    float growth = growthFromKernels(uv);
    color = clamp(color + uniDelaTime * growth, 0.0, 1.0);
    frag_color = vec4(color, color, color, 1.0);
}
