#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec2  u_texelSize;   // 1/texture size
uniform float u_radius;      // 2..4 (try 3)
uniform float u_strength;    // 1..3 (try 2)

out vec4 finalColor;

float luminance(vec3 c){ return dot(c, vec3(0.2126, 0.7152, 0.0722)); }

// Mask from sprite color/alpha (treat bright colors as emitting, but respect alpha)
float spriteMask(vec4 s){
    float y = luminance(s.rgb);
    // soft “emissive” from color; clamp by alpha so holes don’t glow
    float m = clamp(y, 0.0, 1.0) * s.a;
    return m;
}

void main(){
    vec4 base = texture(texture0, fragTexCoord) * fragColor;

    // Build blurred mask (small gaussian around current pixel)
    float acc = 0.0;
    float wsum = 0.0;

    // kernel weights (approx Gaussian) at 0,1,2 steps
    float w0 = 0.227027;
    float w1 = 0.194594;
    float w2 = 0.121621;

    vec2 px = u_texelSize * max(u_radius, 1.0);

    // center
    float m0 = spriteMask(texture(texture0, fragTexCoord) * fragColor);
    acc += m0 * w0; wsum += w0;

    // 1-step cross
    acc += spriteMask(texture(texture0, fragTexCoord + vec2( px.x, 0)) * fragColor) * w1; wsum += w1;
    acc += spriteMask(texture(texture0, fragTexCoord + vec2(-px.x, 0)) * fragColor) * w1; wsum += w1;
    acc += spriteMask(texture(texture0, fragTexCoord + vec2(0,  px.y)) * fragColor) * w1; wsum += w1;
    acc += spriteMask(texture(texture0, fragTexCoord + vec2(0, -px.y)) * fragColor) * w1; wsum += w1;

    // 2-step cross
    acc += spriteMask(texture(texture0, fragTexCoord + vec2( 2.0*px.x, 0)) * fragColor) * w2; wsum += w2;
    acc += spriteMask(texture(texture0, fragTexCoord + vec2(-2.0*px.x, 0)) * fragColor) * w2; wsum += w2;
    acc += spriteMask(texture(texture0, fragTexCoord + vec2(0,  2.0*px.y)) * fragColor) * w2; wsum += w2;
    acc += spriteMask(texture(texture0, fragTexCoord + vec2(0, -2.0*px.y)) * fragColor) * w2; wsum += w2;

    float blurred = (wsum > 0.0) ? acc / wsum : 0.0;

    // outside-only halo field: blur minus original mask
    // this is >0 just outside edges, ≈0 inside solid areas
    float halo = max(blurred - m0, 0.0) * u_strength;

    // Color of the halo derived from sprite color near the edge (sample once)
    vec3 haloRGB = base.rgb;

    // Compose: keep sprite as-is; add halo outside
    vec3 outRGB = base.rgb + haloRGB * halo;
    finalColor = vec4(outRGB, base.a);
}