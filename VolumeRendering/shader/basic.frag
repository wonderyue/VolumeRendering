#version 330 core

in vec3 TexCoord;
in vec4 OutColor;

uniform sampler3D tex3D;
uniform sampler1D tex1D;
uniform bool isWireframe;
uniform float transferValues[8];
uniform float sampleRate0;
uniform float sampleRate;


out vec4 color;

float transfer(float intensity)
{
    int i = int(intensity * 7);
    float t = intensity * 7 - i;
    float alpha = transferValues[i-1] + t * (transferValues[i] - transferValues[i-1]);
    alpha = 1 - pow(1 - alpha, sampleRate0 / sampleRate);
    return alpha;
}

void main()
{
    if (isWireframe)
    {
        color = OutColor;
    }
    else
    {
        float intensity = texture(tex3D, TexCoord).r;
        color = vec4(texture(tex1D, intensity).rgb, transfer(intensity));
    }
}
