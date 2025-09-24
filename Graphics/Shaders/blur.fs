#version 330
in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec2 u_texelSize;   // 1.0 / (texture size of current RT)
uniform vec2 u_direction;   // (1,0)=horizontal, (0,1)=vertical
uniform float u_radius;     // e.g. 2.5..6.0, try 4.0

out vec4 finalColor;

// 9-tap gaussian-ish kernel
void main(){
    vec2 px = u_texelSize * max(u_radius, 1.0);
    vec3 acc = vec3(0.0);
    float wsum = 0.0;

    float w0 = 0.227027;
    float w1 = 0.194594;
    float w2 = 0.121621;
    float w3 = 0.054054;
    float w4 = 0.016216;

    vec2 off0 = vec2(0.0);
    vec2 off1 = u_direction * px * 1.0;
    vec2 off2 = u_direction * px * 2.0;
    vec2 off3 = u_direction * px * 3.0;
    vec2 off4 = u_direction * px * 4.0;

    acc += texture(texture0, fragTexCoord + off0).rgb * w0; wsum += w0;
    acc += texture(texture0, fragTexCoord + off1).rgb * w1; wsum += w1;
    acc += texture(texture0, fragTexCoord - off1).rgb * w1; wsum += w1;
    acc += texture(texture0, fragTexCoord + off2).rgb * w2; wsum += w2;
    acc += texture(texture0, fragTexCoord - off2).rgb * w2; wsum += w2;
    acc += texture(texture0, fragTexCoord + off3).rgb * w3; wsum += w3;
    acc += texture(texture0, fragTexCoord - off3).rgb * w3; wsum += w3;
    acc += texture(texture0, fragTexCoord + off4).rgb * w4; wsum += w4;
    acc += texture(texture0, fragTexCoord - off4).rgb * w4; wsum += w4;

    finalColor = vec4(acc / wsum, 1.0);
}