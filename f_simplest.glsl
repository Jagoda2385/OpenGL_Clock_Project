#version 330

in vec3 normalDir;
in vec2 texCoord;
in vec3 fragPos;
in vec3 viewPos;

uniform vec4 color;

uniform vec4 lp1;          // pozycja światła 1
uniform vec4 lp2;          // pozycja światła 2
uniform vec3 lightColor1;  // kolor światła 1
uniform vec3 lightColor2;  // kolor światła 2

uniform sampler2D tex;
uniform int useTexture;

out vec4 outColor;

void main() {
    vec3 N = normalize(normalDir);
    vec3 V = normalize(viewPos - fragPos);

    // Światło 1
    vec3 L1 = normalize(lp1.xyz - fragPos);
    vec3 R1 = reflect(-L1, N);
    float diff1 = max(dot(N, L1), 0.0);
    float spec1 = pow(max(dot(R1, V), 0.0), 64.0);

    // Światło 2
    vec3 L2 = normalize(lp2.xyz - fragPos);
    vec3 R2 = reflect(-L2, N);
    float diff2 = max(dot(N, L2), 0.0);
    float spec2 = pow(max(dot(R2, V), 0.0), 64.0);

    vec4 baseColor = (useTexture == 1) ? texture(tex, texCoord) : color;

    vec3 ambient = 0.3 * baseColor.rgb;
    vec3 diffuse = diff1 * lightColor1 * baseColor.rgb + diff2 * lightColor2 * baseColor.rgb;
    vec3 specular = spec1 * lightColor1 + spec2 * lightColor2;

    outColor = vec4(ambient + diffuse + specular, baseColor.a);
}
