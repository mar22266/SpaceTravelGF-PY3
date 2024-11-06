#pragma once

#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include "FastNoise.h"
#include "uniforms.h"
#include "fragment.h"
#include "noise.h"
#include "print.h"

static int frame = 0;

Vertex vertexShader(const Vertex &vertex, const Uniforms &uniforms)
{
    // Apply transformations to the input vertex using the matrices from the uniforms
    glm::vec4 clipSpaceVertex = uniforms.projection * uniforms.view * uniforms.model * glm::vec4(vertex.position, 1.0f);

    // Perspective divide
    glm::vec3 ndcVertex = glm::vec3(clipSpaceVertex) / clipSpaceVertex.w;

    // Apply the viewport transform
    glm::vec4 screenVertex = uniforms.viewport * glm::vec4(ndcVertex, 1.0f);

    // Transform the normal
    glm::vec3 transformedNormal = glm::mat3(uniforms.model) * vertex.normal;
    transformedNormal = glm::normalize(transformedNormal);

    glm::vec3 transformedWorldPosition = glm::vec3(uniforms.model * glm::vec4(vertex.position, 1.0f));

    // Return the transformed vertex as a vec3
    return Vertex{
        glm::vec3(screenVertex),
        transformedNormal,
        vertex.tex,
        transformedWorldPosition,
        vertex.position};
}

Fragment rockyPlanetShader(Fragment &fragment)
{
    // Define a range of earthy tones for a more complex surface appearance
    glm::vec3 baseColor = glm::vec3(0.48f, 0.42f, 0.35f);      // Primary rock color
    glm::vec3 secondaryColor = glm::vec3(0.58f, 0.52f, 0.45f); // Secondary rock color
    glm::vec3 highlightColor = glm::vec3(0.75f, 0.70f, 0.65f); // Highlights for edges and ridges
    glm::vec3 shadowColor = glm::vec3(0.30f, 0.28f, 0.26f);    // Shadows in crevices

    // Convert 3D position on sphere to 2D UV coordinates for texturing
    glm::vec3 pos = glm::normalize(fragment.originalPos);
    float u = 0.5f + atan2(pos.z, pos.x) / (2.0f * glm::pi<float>());
    float v = 0.5f - asin(pos.y) / glm::pi<float>();
    glm::vec2 uv = glm::vec2(u, v);
    uv = glm::clamp(uv, 0.0f, 1.0f);

    // Setup noise generators for layered texturing
    FastNoiseLite noiseGenerator1, noiseGenerator2;
    noiseGenerator1.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noiseGenerator2.SetNoiseType(FastNoiseLite::NoiseType_Cellular);

    float scale1 = 100.0f; // Large-scale features
    float scale2 = 400.0f; // Fine details

    // Generate noise values for texture variation
    float noiseValue1 = noiseGenerator1.GetNoise((uv.x + 0.1f) * scale1, (uv.y + 0.1f) * scale1);
    float noiseValue2 = noiseGenerator2.GetNoise((uv.x + 0.5f) * scale2, (uv.y + 0.5f) * scale2);

    // Normalize and adjust noise values
    noiseValue1 = (noiseValue1 + 1.0f) * 0.5f;
    noiseValue2 = (noiseValue2 + 1.0f) * 0.5f;

    // Combine base and secondary colors based on noise
    glm::vec3 mixedColor = glm::mix(baseColor, secondaryColor, noiseValue1);

    // Apply highlight or shadow based on higher frequency detail noise
    if (noiseValue2 > 0.8)
    {
        mixedColor = glm::mix(mixedColor, highlightColor, (noiseValue2 - 0.8f) * 5.0f); // Emphasize highlights
    }
    else if (noiseValue2 < 0.2)
    {
        mixedColor = glm::mix(mixedColor, shadowColor, (0.2f - noiseValue2) * 5.0f); // Emphasize shadows
    }

    // Apply ambient occlusion based on noise to simulate lighting complexity
    mixedColor *= glm::smoothstep(0.2f, 1.0f, noiseValue1);

    // Modulate the final color with the lighting intensity for realism
    glm::vec3 finalColor = mixedColor * fragment.intensity;

    // Convert glm::vec3 color to your Color class, assuming a Color constructor that accepts floats
    fragment.color = Color(finalColor.r, finalColor.g, finalColor.b);

    return fragment;
}
Fragment gasGiantShader(Fragment &fragment)
{
    // Grayscale color definitions for atmospheric effects
    glm::vec3 highAltitude = glm::vec3(0.9f, 0.9f, 0.9f); // Very light gray, almost white
    glm::vec3 midAltitude = glm::vec3(0.6f, 0.6f, 0.6f);  // Medium gray
    glm::vec3 lowAltitude = glm::vec3(0.3f, 0.3f, 0.3f);  // Dark gray
    glm::vec3 stormColor = glm::vec3(0.1f, 0.1f, 0.1f);   // Very dark gray, almost black

    // Convert 3D position to 2D UV coordinates
    glm::vec3 pos = glm::normalize(fragment.originalPos);
    float u = 0.5f + atan2(pos.z, pos.x) / (4.0f * glm::pi<float>());
    float v = 0.5f - asin(pos.y) / glm::pi<float>();
    glm::vec2 uv = glm::vec2(u, v);
    uv = glm::clamp(uv, 0.0f, 1.0f);

    // Initialize noise generator for atmospheric detailing
    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

    float offsetX = 100.0f;  // X offset for noise
    float offsetY = 2000.0f; // Y offset for noise
    float scale = 800.0f;    // Scale factor for noise to control the detail level

    // Calculate noise value for atmospheric layers
    float atmosphericNoise = noiseGenerator.GetNoise((uv.x + offsetX) * scale, (uv.y + offsetY) * scale);
    atmosphericNoise = glm::smoothstep(0.0f, 1.0f, atmosphericNoise);

    // Define the base color based on atmospheric noise
    glm::vec3 baseColor = glm::mix(lowAltitude, highAltitude, atmosphericNoise);

    // Additional noise layer for storm effects
    float stormNoise = noiseGenerator.GetNoise((uv.x + offsetX * 0.5f) * scale * 2, (uv.y + offsetY * 0.5f) * scale * 2);
    stormNoise = glm::smoothstep(0.8f, 1.0f, stormNoise); // Sharper transition to create distinct storm features

    // Integrate storm effects into the atmosphere
    if (stormNoise > 0.9)
    {
        baseColor = glm::mix(baseColor, stormColor, (stormNoise - 0.9f) * 10.0f);
    }

    // Apply cloud-like variations using a high frequency noise layer
    float cloudVariation = noiseGenerator.GetNoise(uv.x * scale * 3, uv.y * scale * 3);
    cloudVariation = glm::smoothstep(0.7f, 1.0f, cloudVariation);
    baseColor = glm::mix(baseColor, midAltitude, cloudVariation);

    // Modulate the final color by the lighting intensity to simulate lighting effects
    baseColor *= fragment.intensity;

    // Convert the final calculated color to the Color class
    fragment.color = Color(baseColor.r, baseColor.g, baseColor.b);

    return fragment;
}

Fragment sunShader(Fragment &fragment)
{
    // Define solar color gradients focusing exclusively on shades of orange
    glm::vec3 coreOrange = glm::vec3(1.0f, 0.7f, 0.2f);  // Vibrant orange for the core
    glm::vec3 midOrange = glm::vec3(1.0f, 0.5f, 0.1f);   // Mid-range orange for general surface
    glm::vec3 darkOrange = glm::vec3(1.0f, 0.4f, 0.0f);  // Darker orange for outer regions
    glm::vec3 flareOrange = glm::vec3(1.0f, 0.3f, 0.0f); // Bright orange for flares and active areas

    // Convert 3D position to 2D UV coordinates
    glm::vec3 pos = glm::normalize(fragment.originalPos);
    float u = 0.5f + atan2(pos.z, pos.x) / (2.0f * glm::pi<float>());
    float v = 0.5f - asin(pos.y) / glm::pi<float>();
    glm::vec2 uv = glm::vec2(u, v);

    // Set up noise generator for dynamic surface effects
    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

    float offsetX = 8000.0f; // Position offset for noise
    float offsetY = 1000.0f;
    float scale = 30000.0f; // Large scale to enhance feature visibility

    // Generate noise value for dynamic solar surface effects
    float noiseValue = noiseGenerator.GetNoise((uv.x + offsetX) * scale, (uv.y + offsetY) * scale);
    noiseValue = (noiseValue + 1.0f) * 0.5f; // Normalize to [0, 1]

    glm::vec3 baseColor = glm::mix(coreOrange, midOrange, noiseValue); // Blend between core and midrange

    // Sinusoidal function to simulate flares and surface activity
    float sinValue = glm::sin(uv.y * 40.0f + noiseValue * 100.0f) * 0.1f + 0.9f;
    glm::vec3 sinColor = glm::mix(midOrange, flareOrange, sinValue); // Introduce bright flare effects

    // Blend base color with sinusoidal color based on noise value
    if (noiseValue > 0.95f)
    {
        baseColor = glm::mix(baseColor, sinColor, (noiseValue - 0.95f) * 20.0f);
    }
    else if (noiseValue > 0.8f)
    {
        baseColor = glm::mix(baseColor, darkOrange, (noiseValue - 0.8f) * 5.0f);
    }

    // Modulate final color by lighting intensity for realistic rendering
    glm::vec3 finalColor = baseColor * fragment.intensity;

    // Convert the final calculated color to the Color class
    fragment.color = Color(finalColor.r, finalColor.g, finalColor.b);

    return fragment;
}

Fragment earthShader(Fragment &fragment)
{
    // Updated variable names for color definitions
    glm::vec3 lushGreen = glm::vec3(0.12f, 0.48f, 0.12f);   // Richer forest green
    glm::vec3 deepBlue = glm::vec3(0.0f, 0.0f, 0.9f);       // Deep blue for oceans
    glm::vec3 verdantGreen = glm::vec3(0.0f, 0.28f, 0.20f); // A darker shade of green
    glm::vec3 soilBrown = glm::vec3(0.45f, 0.30f, 0.20f);   // Darker brown for soil
    glm::vec3 cumulusWhite = glm::vec3(1.0f, 1.0f, 1.0f);   // Pure white for clouds

    // Convert 3D position to 2D UV coordinates
    glm::vec3 pos = glm::normalize(fragment.originalPos);
    float u = 0.5f + atan2(pos.z, pos.x) / (4.0f * glm::pi<float>());
    float v = 0.5f - asin(pos.y) / glm::pi<float>();
    glm::vec2 uv = glm::vec2(u, v);
    uv = glm::clamp(uv, 0.0f, 1.0f);

    // Initialize noise generator for terrain detailing
    FastNoiseLite terrainNoise;
    terrainNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    float offsetX = 100.0f;
    float offsetY = 200000.0f;
    float scale = 600.0f;

    // Calculate noise value for different terrain types
    float noiseValue = terrainNoise.GetNoise((uv.x + offsetX) * scale, (uv.y + offsetY) * scale);
    glm::vec3 baseColor;

    if (noiseValue < 0.4f)
    {
        // Water areas
        baseColor = glm::mix(deepBlue, verdantGreen, noiseValue);
        baseColor = glm::smoothstep(glm::vec3(0.0f), glm::vec3(0.9f), baseColor);
    }
    else if (noiseValue < 0.6f)
    {
        // Shallow water or coastal areas
        baseColor = verdantGreen;
    }
    else if (noiseValue < 0.8f)
    {
        // Vegetation and forest areas
        baseColor = lushGreen;
    }
    else if (noiseValue < 0.9f)
    {
        // Elevated terrain and rocky areas
        baseColor = soilBrown;
    }
    else
    {
        // High-altitude areas with sparse vegetation
        baseColor = deepBlue;
    }

    // Generate cloud overlay using higher-frequency noise
    float cloudOverlay = terrainNoise.GetNoise((uv.x + offsetX) * scale * 0.5f, (uv.y + offsetY) * scale * 0.5f);
    cloudOverlay = (cloudOverlay + 1.0f) * 0.3f;
    cloudOverlay = glm::smoothstep(0.0f, 1.0f, cloudOverlay);

    // Mix cloud color based on the cloud noise
    baseColor = glm::mix(baseColor, cumulusWhite, cloudOverlay);

    // Convert the final color to the Color class used in your graphics engine
    fragment.color = Color(baseColor.r, baseColor.g, baseColor.b);

    return fragment;
}

Fragment marsShader(Fragment &fragment)
{
    // Define the colors specifically for the Martian landscape
    glm::vec3 primaryRed = glm::vec3(0.7f, 0.2f, 0.1f); // Dominant reddish soil
    glm::vec3 darkBrown = glm::vec3(0.4f, 0.1f, 0.05f); // Darker patches of soil
    glm::vec3 orangeTint = glm::vec3(0.8f, 0.4f, 0.1f); // Orange hues for variation
    glm::vec3 lightRed = glm::vec3(0.8f, 0.3f, 0.2f);   // Lighter, dusty areas

    // Convert 3D position on sphere to 2D UV
    glm::vec3 pos = glm::normalize(fragment.originalPos);
    float u = 0.5f + atan2(pos.z, pos.x) / (4.0f * glm::pi<float>());
    float v = 0.5f - asin(pos.y) / glm::pi<float>();
    glm::vec2 uv = glm::vec2(u, v);

    uv = glm::clamp(uv, 0.0f, 1.0f);

    // Set up the noise generator for terrain detailing
    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

    float offsetX = 100.0f;
    float offsetY = 200.0f;
    float scale = 500.0f;

    // Generate the noise value for terrain variation
    float noiseValue = noiseGenerator.GetNoise((uv.x + offsetX) * scale, (uv.y + offsetY) * scale);
    glm::vec3 c;

    // Determine terrain colors based on noise value
    if (noiseValue < 0.4f)
    {
        // Transition to dark brown for lower regions
        c = glm::mix(darkBrown, primaryRed, noiseValue);
    }
    else if (noiseValue < 0.6f)
    {
        // Main reddish soil
        c = primaryRed;
    }
    else if (noiseValue < 0.8f)
    {
        // Mix in orange for variation in higher areas
        c = glm::mix(primaryRed, orangeTint, (noiseValue - 0.6f) * 2.5f);
    }
    else
    {
        // Light dusty areas on the highest terrains
        c = glm::mix(orangeTint, lightRed, (noiseValue - 0.8f) * 5.0f);
    }

    // Apply lighting intensity for realism
    c *= fragment.intensity;

    // Convert glm::vec3 color to your Color class
    fragment.color = Color(c.r, c.g, c.b);

    return fragment;
}

Fragment neptuneShader(Fragment &fragment)
{
    // Atmospheric color definitions
    glm::vec3 deepBlue = glm::vec3(0.0f, 0.0f, 0.3f);
    glm::vec3 lightBlue = glm::vec3(0.2f, 0.2f, 0.7f);
    glm::vec3 vividPurple = glm::vec3(0.7f, 0.1f, 0.7f);
    glm::vec3 polarWhite = glm::vec3(1.0f, 1.0f, 1.0f);

    // Normalize position to unit sphere for UV calculation
    glm::vec3 pos = glm::normalize(fragment.originalPos);
    float u = 0.5f + atan2(pos.z, pos.x) / (2.0f * glm::pi<float>());
    float v = 0.5f - asin(pos.y) / glm::pi<float>();
    glm::vec2 uv = glm::vec2(u, v);
    uv = glm::clamp(uv, 0.0f, 1.0f);

    // Noise generation for atmospheric and cloud patterns
    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    float cloudNoise = noiseGenerator.GetNoise(pos.x * 400.0f, pos.y * 400.0f, pos.z * 400.0f);
    cloudNoise = glm::smoothstep(0.2f, 0.6f, cloudNoise);

    glm::vec3 baseColor = glm::mix(deepBlue, lightBlue, cloudNoise);

    // Ring calculations
    float ringInnerRadius = 1.1f; // Inner radius of the ring relative to planet radius
    float ringOuterRadius = 1.3f; // Outer radius of the ring
    float ringThickness = 0.05f;  // Thickness of the ring in relative units

    // Cylindrical radius on the planet's equatorial plane
    float cylindricalRadius = glm::length(glm::vec2(pos.x, pos.z));
    float heightFromEquator = glm::abs(pos.y);

    // Check if the fragment is within the ring's volume
    if (cylindricalRadius > ringInnerRadius && cylindricalRadius < ringOuterRadius && heightFromEquator < ringThickness)
    {
        baseColor = polarWhite; // Color the ring white
    }

    // Modulate final color by lighting intensity
    glm::vec3 finalColor = baseColor * fragment.intensity;

    // Convert the final calculated color to the Color class
    fragment.color = Color(finalColor.r, finalColor.g, finalColor.b);

    return fragment;
}

Fragment starShader(Fragment &fragment)
{
    Color color;

    glm::vec3 secondColor = glm::vec3(55.0f / 255.0f, 0.0f / 255.0f, 55.0f / 255.0f);
    glm::vec3 mainColor = glm::vec3(255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f);

    glm::vec2 uv = glm::vec2(fragment.originalPos.x * 2.0 - 1.0, fragment.originalPos.y * 2.0 - 1.0);

    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

    float offsetX = 8000.0f;
    float offsetY = 1000.0f;
    float scale = 30000.0f;

    // Genera el valor de ruido
    float noiseValue = noiseGenerator.GetNoise((uv.x + offsetX) * scale, (uv.y + offsetY) * scale);
    noiseValue = (noiseValue + 1.0f) * 0.9f;

    // Interpola entre el color base y el color secundario basado en el valor de ruido
    secondColor = glm::mix(mainColor, secondColor, noiseValue);

    if (noiseValue > 0.99f)
    {
        // Calcula el valor sinusoide para crear líneas
        float sinValue = glm::sin(uv.y * 20.0f) * 0.1f;

        sinValue = glm::smoothstep(0.8f, 1.0f, sinValue);

        // Combina el color base con las líneas sinusoide
        secondColor = secondColor + glm::vec3(sinValue);
    }

    // Interpola entre el color base y el color secundario basado en el valor de ruido
    mainColor = glm::mix(mainColor, mainColor, noiseValue);

    // Interpola entre el color base y el color secundario basado en el valor de ruido
    secondColor = glm::mix(mainColor, secondColor, noiseValue);

    color = Color(secondColor.x, secondColor.y, secondColor.z);

    fragment.color = color;

    return fragment;
}
