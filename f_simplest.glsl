#version 330

in vec3 normalDir;
in vec2 texCoord;
in vec3 fragPos;
in vec3 viewPos;

uniform vec4 color;
uniform vec4 lp;
uniform sampler2D tex;
uniform int useTexture;

out vec4 outColor;

void main() {
    vec3 N = normalize(normalDir);
    vec3 L = normalize(lp.xyz - fragPos);
    vec3 V = normalize(viewPos - fragPos);
    vec3 R = reflect(-L, N); // odbite światło

    float diff = max(dot(N, L), 0.0);
    float spec = pow(max(dot(R, V), 0.0), 64.0); // połysk

    vec4 baseColor = (useTexture == 1) ? texture(tex, texCoord) : color;

    vec3 ambient = 0.3 * baseColor.rgb;
    vec3 diffuse = 1.0 * diff * baseColor.rgb;
    vec3 specular = 0.7 * spec * vec3(1.0); // jasny biały błysk

    outColor = vec4(ambient + diffuse + specular, baseColor.a);
}
