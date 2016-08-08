#version 330 core
// opengl 3.2 -> glsl version1.50

in vec3 v2fColor;
in vec2 v2fUV;

uniform vec4 baseColor;

uniform sampler2D mainTexture;
uniform sampler2D secondTexture;

out vec4 outColor;

void main() {
    // outColor = vec4(1,1,1,1) - vec4(v2fColor, 0.1) * vec4(triangleColor, 0.1);
    // outColor = texture(mainTexture, v2fUV) * vec4(v2fColor, 1.1);
    vec4 texColor = mix(
                        texture(mainTexture, vec2(v2fUV.x, 1-v2fUV.y)),
                        texture(secondTexture, vec2(v2fUV.x, 1-v2fUV.y)),
                        0.5);
    vec4 simpleColor = baseColor * vec4(v2fColor, 1.0);
    outColor = texColor * simpleColor;
    // outColor = texture(mainTexture, vec2(v2fUV.x, 1-v2fUV.y));
}
