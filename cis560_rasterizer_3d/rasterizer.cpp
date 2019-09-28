#include "rasterizer.h"
#include "line.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <float.h>
#include <iostream>

glm::vec3 BarycentricInterpolation(Vertex iv1, Vertex iv2, Vertex iv3, glm::vec3 iPos);
float PerspectiveInterpolationZ(glm::vec3 iP1, glm::vec3 iP2, glm::vec3 iP3, glm::vec3 iPos);
float PerspectiveInterpolationC(glm::vec4 iP1, glm::vec4 iP2, glm::vec4 iP3, glm::vec3 iPos);
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

                    glm::vec3 s1(s_p1.x, s_p1.y, s_p1.z);
                    glm::vec3 s2(s_p2.x, s_p2.y, s_p2.z);
                    glm::vec3 s3(s_p3.x, s_p3.y, s_p3.z);

                    // Zp is the depth in the camera space.
                    float Zp = PerspectiveInterpolationZ(glm::vec3(P1x, P1y, v_p1.z), glm::vec3(P2x, P2y, v_p2.z), glm::vec3(P3x, P3y, v_p3.z), glm::vec3(iX, iY, iZ));

                    glm::vec2 UVp;
                    UVp.x = PerspectiveInterpolationC(glm::vec4(P1x, P1y, v_p1.z, v1.m_uv.x),glm::vec4(P2x, P2y, v_p2.z, v2.m_uv.x),glm::vec4(P3x, P3y, v_p3.z, v3.m_uv.x), glm::vec3(iX, iY, Zp));
                    UVp.y = PerspectiveInterpolationC(glm::vec4(P1x, P1y, v_p1.z, v1.m_uv.y),glm::vec4(P2x, P2y, v_p2.z, v2.m_uv.y),glm::vec4(P3x, P3y, v_p3.z, v3.m_uv.y), glm::vec3(iX, iY, Zp));


                    glm::vec3 normalP;
                    normalP.x = PerspectiveInterpolationC(glm::vec4(P1x, P1y, v_p1.z, v1.m_normal.x),glm::vec4(P2x, P2y, v_p2.z, v2.m_normal.x),glm::vec4(P3x, P3y, v_p3.z, v3.m_normal.x), glm::vec3(iX, iY, Zp));
                    normalP.y = PerspectiveInterpolationC(glm::vec4(P1x, P1y, v_p1.z, v1.m_normal.y),glm::vec4(P2x, P2y, v_p2.z, v2.m_normal.y),glm::vec4(P3x, P3y, v_p3.z, v3.m_normal.y), glm::vec3(iX, iY, Zp));
                    normalP.z = PerspectiveInterpolationC(glm::vec4(P1x, P1y, v_p1.z, v1.m_normal.z),glm::vec4(P2x, P2y, v_p2.z, v2.m_normal.z),glm::vec4(P3x, P3y, v_p3.z, v3.m_normal.z), glm::vec3(iX, iY, Zp));


                    // Simple cliping.
                    if ((iX < 0 || iX >= 512) || (iY < 0 || iY >= 512) || (Zp <= 0) || (Zp >= 10000))
                    {
                        continue;
                    }

                    float tempZ = Zp;

                    float width = tarPoly.mp_texture->width();
                    float length = tarPoly.mp_texture->height();

                    // Map UV value to the texture pixel pos.
                    float texturePosU = (width - 1) * UVp.x;
                    float texturePosV = (length - 1) * UVp.y;
                    QColor tempColor = tarPoly.mp_texture->pixelColor(texturePosU, texturePosV);

                    if(tempZ < zBuffer.at(i + yRow * 512))
                    {
                        float beforeOverrideDepth = zBuffer[i + yRow * 512];
                        zBuffer[i + yRow * 512] = tempZ;

                        result.setPixel(i, yRow, qRgb(tempColor.red(), tempColor.green(), tempColor.blue()));
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

float PerspectiveInterpolationC(glm::vec4 iP1, glm::vec4 iP2, glm::vec4 iP3, glm::vec3 iPos)
{
    glm::vec3 p1 = glm::vec3(iP1.x, iP1.y, 0.f);
    glm::vec3 p2 = glm::vec3(iP2.x, iP2.y, 0.f);
    glm::vec3 p3 = glm::vec3(iP3.x, iP3.y, 0.f);


    float c1 = iP1.w;
    float c2 = iP2.w;
    float c3 = iP3.w;

    float z = iPos.z;
    float z1 = iP1.z;
    float z2 = iP2.z;
    float z3 = iP3.z;
    iPos.z = 0.f;

    glm::vec3 v1_2 = p2 - p1;
    glm::vec3 v1_3 = p3 - p1;
    float s = glm::length(glm::cross(v1_2, v1_3)) / 2;

    glm::vec3 v1_iPos = iPos - p1;
    float s2 = glm::length(glm::cross(v1_iPos, v1_3)) / 2;
    float s3 = glm::length(glm::cross(v1_iPos, v1_2)) / 2;

    glm::vec3 viPos_2 = p2 - iPos;
    glm::vec3 viPos_3 = p3 - iPos;
    float s1 = glm::length(glm::cross(viPos_2, viPos_3)) / 2;

    float c = z * ((c1 * s1) / (z1 * s) + (c2 * s2) / (z2 * s) + (c3 * s3) / (z3 * s));

    return c;
}

float PerspectiveInterpolationZ(glm::vec3 iP1, glm::vec3 iP2, glm::vec3 iP3, glm::vec3 iPos) //(screen x, screen y, world z);
{
    float z1 = iP1.z;
    float z2 = iP2.z;
    float z3 = iP3.z;

    iP1.z = 0.f;
    iP2.z = 0.f;
    iP3.z = 0.f;
    iPos.z = 0.f;

    glm::vec3 p1 = iP1;
    glm::vec3 p2 = iP2;
    glm::vec3 p3 = iP3;

    glm::vec3 v1_2 = p2 - p1;
    glm::vec3 v1_3 = p3 - p1;
    float s = glm::length(glm::cross(v1_2, v1_3)) / 2;

    glm::vec3 v1_iPos = iPos - p1;
    float s2 = glm::length(glm::cross(v1_iPos, v1_3)) / 2;
    float s3 = glm::length(glm::cross(v1_iPos, v1_2)) / 2;

    glm::vec3 viPos_2 = p2 - iPos;
    glm::vec3 viPos_3 = p3 - iPos;
    float s1 = glm::length(glm::cross(viPos_2, viPos_3)) / 2;

    float ele1, ele2, ele3;
    if((z1 * s) < FLT_EPSILON)
    {
        ele1 = 0.f;
    }else{
        ele1 = s1 / (z1 * s);
    }

    if((z2 * s) < FLT_EPSILON)
    {
        ele2 = 0.f;
    }else{
        ele2 = s2 / (z2 * s);
    }

    if((z3 * s) < FLT_EPSILON)
    {
        ele3 = 0.f;
    }else{
        ele3 = s3 / (z3 * s);
    }


    float result = ele1 + ele2 + ele3;
    if(result < FLT_EPSILON)
    {
        return 0.f;
    }else{
        return (1 / result);
    }

    /*
    float top = z1 * z2 * z3 * s;
    float bottom1 = s1 * z2 * z3;
    float bottom2 = s2 * z1 * z3;
    float bottom3 = s3 * z2 * z1;

    if((bottom1 + bottom2 + bottom3) < FLT_EPSILON )
    {
        return 0.f;
    }else{
        return (top / (bottom1 + bottom2 + bottom3));
    }*/
}
/*
glm::vec3 BarycentricInterpolationScreenSpace(glm::vec3 iP1, glm::vec3 iP2, glm::vec3 iP3, glm::vec3 iPos)
{
    glm::vec3 p1 = iP1;
    glm::vec3 p2 = iP2;
    glm::vec3 p3 = iP3;

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
*/
glm::vec3 BarycentricInterpolation(Vertex iv1, Vertex iv2, Vertex iv3, glm::vec3 iPos)
{
    struct Vertex v1 = iv1;
    struct Vertex v2 = iv2;
    struct Vertex v3 = iv3;

    glm::vec3 p1(v1.m_pos.x, v1.m_pos.y, 0.f);
    glm::vec3 p2(v2.m_pos.x, v2.m_pos.y, 0.f);
    glm::vec3 p3(v3.m_pos.x, v3.m_pos.y, 0.f);

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


