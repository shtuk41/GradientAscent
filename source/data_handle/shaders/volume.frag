#version 330 core

in vec3 TexCoord;
out vec4 FragColor;

uniform sampler3D volumeTex;
uniform mat4 invMVP;
uniform vec3 cameraPos;

void main()
{
    // 1. Convert gl_FragCoord.xy from screen pixel coords to NDC [-1,1]
    vec2 ndc = (gl_FragCoord.xy / vec2(textureSize(volumeTex, 0).xy)) * 2.0 - 1.0;
    
    // 2. Use actual depth from gl_FragCoord.z (0 to 1)
    float depth = gl_FragCoord.z;

    // 3. Reconstruct clip space position
    vec4 clip = vec4(ndc, depth, 1.0);

    // 4. Transform to world space
    vec4 worldPos = invMVP * clip;
    worldPos /= worldPos.w;

    // 5. Compute ray direction from camera to worldPos
    vec3 rayDir = normalize(worldPos.xyz - cameraPos);

    // 6. Raymarch parameters
    vec3 pos = TexCoord;
    float stepSize = 0.01;
    float accumulated = 0.0;

    for (int i = 0; i < 100; i++)
    {
        // Sample volume
        float sample = texture(volumeTex, pos).r;

        // Accumulate color (simple additive)
        accumulated += sample * stepSize * 5;

        // Advance ray
        pos += rayDir * stepSize;

        // Stop if outside volume bounds [0,1]
        if (any(lessThan(pos, vec3(0.0))) || any(greaterThan(pos, vec3(1.0))))
            break;
    }

    // Output final color with alpha
    //FragColor = vec4(vec3(accumulated), accumulated * 0.8);
    FragColor = vec4(vec3(accumulated,0,0), accumulated * 0.8);
}
