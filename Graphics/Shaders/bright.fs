#version 330
in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;   // scene color
uniform float u_threshold;    // e.g. 0.6
uniform float u_knee;         // e.g. 0.2

out vec4 finalColor;

float luma(vec3 c){ return dot(c, vec3(0.2126,0.7152,0.0722)); }

void main(){
    vec3 rgb = texture(texture0, fragTexCoord).rgb;
    float y = luma(rgb);
    float t = u_threshold;
    float k = u_knee;
    // soft knee
    float soft = clamp((y - t) / max(k, 1e-4), 0.0, 1.0);
    float m = clamp(y - t, 0.0, 1.0) + soft*(1.0 - clamp(y,0.0,1.0));
    finalColor = vec4(rgb * m, 1.0);
}