#include "rasterizer.h"
#include "line.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <float.h>
#include <iostream>

glm::vec2 PerspectiveUV(glm::vec3 rightarray, glm::vec3 iv1, glm::vec3 iv2, glm::vec3 iv3, glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3, float z);
float PerspectiveZ(glm::vec3 rightarray, float z1, float z2, float z3);
glm::vec3 BerycentricArray(glm::vec3 iv1, glm::vec3 iv2, glm::vec3 iv3, glm::vec3 iPos);
glm::vec3 BerycentricInterpolation(glm::vec3 rightarray, glm::vec3 iv1, glm::vec3 iv2, glm::vec3 iv3, glm::vec3 color1, glm::vec3 color2, glm::vec3 color3, float& z);
glm::vec4 PerspectiveNormal(glm::vec3 rightarray, glm::vec3 iv1, glm::vec3 iv2, glm::vec3 iv3, glm::vec4 inormal1, glm::vec4 inormal2, glm::vec4 inormal3, float z);
float LambertianLightRatio(glm::vec3 wNormal, glm::vec3 wLightDir);

Rasterizer::Rasterizer(const std::vector<Polygon>& polygons)
    : m_polygons(polygons)
{}

QImage Rasterizer::RenderScene()
{
    //glm::rotate()
    QImage result(512, 512, QImage::Format_RGB32);
    std::array<float, 512*512> zBuffer;
    glm::mat4x4 V = m_Camera.GetViewMatrix();
    glm::mat4x4 P = m_Camera.GetPerspectiveProjMatrix();
    glm::mat4x4 VP = m_Camera.GetPerspectiveProjMatrix() * m_Camera.GetViewMatrix();

    glm::mat4x4 correctV = glm::lookAt(glm::vec3(), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    for(int i = 0; i < 512 * 512; i++)
    {
        zBuffer[i] = 100000;
    }
    // Fill the image with black pixels.
    // Note that qRgb creates a QColor,
    // and takes in values [0, 255] rather than [0, 1].
    result.fill(qRgb(0.f, 0.f, 0.f));
    // TODO: Complete the various components of code that make up this function
    // It should return the rasterized image of the current scene

    // Each loop draws a polygon.
    for(int polyIndex = 0; polyIndex < m_polygons.size(); polyIndex++)
    {
        Polygon tarPoly = m_polygons[polyIndex];

        // Each loop draws a triangle.
        for(int triIndex = 0; triIndex < tarPoly.m_tris.size(); triIndex++)
        {
            Triangle tarTri = tarPoly.TriAt(triIndex);
            // Get three points.
            struct Vertex v1 = tarPoly.VertAt(tarTri.m_indices[0]);
            struct Vertex v2 = tarPoly.VertAt(tarTri.m_indices[1]);
            struct Vertex v3 = tarPoly.VertAt(tarTri.m_indices[2]);


            // Get three points' pos in the model space.
            glm::vec4 m_p1 = v1.m_pos;
            glm::vec4 m_p2 = v2.m_pos;
            glm::vec4 m_p3 = v3.m_pos;

            // Get three points' pos in the world space.

            // Get the positions in the normalized coordinate space.
            glm::vec4 n_p1 = VP * v1.m_pos;
            glm::vec4 n_p2 = VP * v2.m_pos;
            glm::vec4 n_p3 = VP * v3.m_pos;

            glm::vec4 v_p1 = V * v1.m_pos;
            glm::vec4 v_p2 = V * v2.m_pos;
            glm::vec4 v_p3 = V * v3.m_pos;

            // Get the positions in the screen space.
            glm::vec4 s_p1 = n_p1 / n_p1.w;
            glm::vec4 s_p2 = n_p2 / n_p2.w;
            glm::vec4 s_p3 = n_p3 / n_p3.w;

            // Get the position in the pixel space.
            float P1x = ((s_p1.x + 1) / 2) * 512;
            float P1y = ((1 - s_p1.y) / 2) * 512;
            glm::vec2 p1(P1x, P1y);

            float P2x = ((s_p2.x + 1) / 2) * 512;
            float P2y = ((1 - s_p2.y) / 2) * 512;
            glm::vec2 p2(P2x, P2y);

            float P3x = ((s_p3.x + 1) / 2) * 512;
            float P3y = ((1 - s_p3.y) / 2) * 512;
            glm::vec2 p3(P3x, P3y);

            glm::vec3 pixelP1(p1, 0.f);
            glm::vec3 pixelP2(p2, 0.f);
            glm::vec3 pixelP3(p3, 0.f);

            glm::vec3 pixelScreenP1(p1, s_p1.z);
            glm::vec3 pixelScreenP2(p2, s_p2.z);
            glm::vec3 pixelScreenP3(p3, s_p3.z);


            BoundingBox triBoundingBox(glm::vec4(P1x, P1y, 0.f, 0.f), glm::vec4(P2x, P2y, 0.f, 0.f), glm::vec4(P3x, P3y, 0.f, 0.f));


            float bigYBound = triBoundingBox.GetBiggestY();
            float smallYBound = triBoundingBox.GetSmallestY();

            // Construct three Edges.
            Line edges[3] = {Line(p1, p2), Line(p1, p3), Line(p2, p3)};

            // Drawing the triangle row by row. smallYBound
            for(float yRow = smallYBound; yRow <= bigYBound; yRow++)
            {
                // Get the two intercepted x values.
                float interceptedx[2];
                unsigned int ptr = 0;
                for(int i = 0; i < 3; i++)
                {
                    if (edges[i].YValueAndLineIntersection(yRow, &interceptedx[ptr]))
                    {
                        ptr++;
                    }
                }

                // Get the small x value, and the big x value.
                float bigerX, smallerX;
                if(interceptedx[0] > interceptedx[1])
                {
                    bigerX = interceptedx[0];
                    smallerX = interceptedx[1];
                }
                else
                {
                    bigerX = interceptedx[1];
                    smallerX = interceptedx[0];
                }

                // Color the pixels between the two x values.
                for(float i = smallerX; i <= bigerX; i += 0.5f)
                {
                    // cannot implicitly use yRow construct vec, because vec is a float datastructure.
                    float iX = float(i);
                    float iY = float(yRow);
                    float iZ = 0.f;
                    glm::vec3 pixelCurrent(iX, iY, iZ);

                    glm::vec3 rightIndex = BerycentricArray(pixelP1, pixelP2, pixelP3, pixelCurrent);

                    float tempZ;
                    glm::vec3 color = BerycentricInterpolation(rightIndex, pixelP1, pixelP2, pixelP3, v1.m_color, v2.m_color, v3.m_color, tempZ);

                    // Simple cliping.
                    if ((iX < 0 || iX >= 512) || (iY < 0 || iY >= 512))
                    {
                        continue;
                    }

                    if(tempZ <= zBuffer.at(i + yRow * 512))
                    {
                        float Zp = PerspectiveZ(rightIndex, s_p1.z, s_p2.z, s_p3.z);

                        glm::vec2 UVp;
                        UVp = PerspectiveUV(rightIndex, pixelScreenP1, pixelScreenP2, pixelScreenP3, v1.m_uv, v2.m_uv, v3.m_uv, Zp);

                        glm::vec4 normalP;
                        normalP = PerspectiveNormal(rightIndex, pixelScreenP1, pixelScreenP2, pixelScreenP3, v1.m_normal, v2.m_normal, v3.m_normal, Zp);

                        float E = glm::dot(glm::normalize(normalP), glm::normalize(-m_Camera.forwardDir));
                        E = glm::clamp(E , 0.f, 1.f);

                        if(UVp.x <= 1 && UVp.x >= 0 && UVp.y <= 1 && UVp.y >= 0)
                        {
                            zBuffer[i + yRow * 512] = Zp;
                            glm::vec3 textureColor = GetImageColor(UVp, tarPoly.mp_texture);
                            glm::vec3 ambientColor = E * textureColor;
                            glm::vec3 colorWithLight = 0.3f * textureColor + ambientColor;
                            float finalColorR = glm::clamp(colorWithLight.r, 0.f, 255.f);
                            float finalColorG = glm::clamp(colorWithLight.g, 0.f, 255.f);
                            float finalColorB = glm::clamp(colorWithLight.b, 0.f, 255.f);

                            result.setPixel(i, yRow, qRgb(finalColorR, finalColorG, finalColorB));
                        }
                    }
                }
            }
        }
    }

    return result;
}

void Rasterizer::ClearScene()
{
    m_polygons.clear();
}

glm::vec2 PerspectiveUV(glm::vec3 rightarray, glm::vec3 iv1, glm::vec3 iv2, glm::vec3 iv3, glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3, float z)
{
    float u = z * (rightarray[0] * uv1[0] / iv1[2] + rightarray[1] * uv2[0] / iv2[2] + rightarray[2] * uv3[0] / iv3[2]);
    float v = z * (rightarray[0] * uv1[1] / iv1[2] + rightarray[1] * uv2[1] / iv2[2] + rightarray[2] * uv3[1] / iv3[2]);
    return glm::vec2(u, v);
}

float PerspectiveZ(glm::vec3 rightarray, float z1, float z2, float z3)
{
    float reciprocal = rightarray[0] / z1 + rightarray[1] / z2 + rightarray[2] / z3;
    return (1 / reciprocal);
}

glm::vec4 PerspectiveNormal(glm::vec3 rightarray, glm::vec3 iv1, glm::vec3 iv2, glm::vec3 iv3, glm::vec4 inormal1, glm::vec4 inormal2, glm::vec4 inormal3, float z)
{
    float n1 = z * (rightarray[0] * inormal1[0] / iv1[2] + rightarray[1] * inormal2[0] / iv2[2] + rightarray[2] * inormal3[0] / iv3[2]);
    float n2 = z * (rightarray[0] * inormal1[1] / iv1[2] + rightarray[1] * inormal2[1] / iv2[2] + rightarray[2] * inormal3[1] / iv3[2]);
    float n3 = z * (rightarray[0] * inormal1[2] / iv1[2] + rightarray[1] * inormal2[2] / iv2[2] + rightarray[2] * inormal3[2] / iv3[2]);
    float n4 = z * (rightarray[0] * inormal1[3] / iv1[2] + rightarray[1] * inormal2[3] / iv2[2] + rightarray[2] * inormal3[3] / iv3[2]);
    return glm::vec4(n1, n2, n3, n4);
}

glm::vec3 Berycentric(glm::vec3 iv1, glm::vec3 iv2, glm::vec3 iv3, glm::vec3 iPos)
{
    glm::vec3 p1(iv1.x, iv1.y, 0.f);
    glm::vec3 p2(iv2.x, iv2.y, 0.f);
    glm::vec3 p3(iv3.x, iv3.y, 0.f);

    glm::vec3 v1_2 = p2 - p1;
    glm::vec3 v1_3 = p3 - p1;
    float s = glm::length(glm::cross(v1_2, v1_3)) / 2;

    glm::vec3 v1_iPos = iPos - p1;
    float s2 = glm::length(glm::cross(v1_iPos, v1_3)) / 2;
    float s3 = glm::length(glm::cross(v1_iPos, v1_2)) / 2;

    glm::vec3 viPos_2 = p2 - iPos;
    glm::vec3 viPos_3 = p3 - iPos;
    float s1 = glm::length(glm::cross(viPos_2, viPos_3)) / 2;

    float r1 = s1 / s;
    float r2 = s2 / s;
    float r3 = s3 / s;
    return glm::vec3(r1, r2, r3);
}

glm::vec3 BerycentricInterpolation(glm::vec3 rightarray, glm::vec3 iv1, glm::vec3 iv2, glm::vec3 iv3, glm::vec3 color1, glm::vec3 color2, glm::vec3 color3, float& z)
{
    float colorR = color1.r * rightarray[0]  + color2.r * rightarray[1] + color3.r * rightarray[2];
    float colorG = color1.g * rightarray[0]  + color2.g * rightarray[1] + color3.g * rightarray[2];
    float colorB = color1.b * rightarray[0]  + color2.b * rightarray[1] + color3.b * rightarray[2];
    z = iv1.y * rightarray[0] + iv2.y * rightarray[1] + iv3.y * rightarray[2];
    return glm::vec3(colorR, colorG, colorB);
}
