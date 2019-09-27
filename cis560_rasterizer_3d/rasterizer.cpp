#include "rasterizer.h"
#include "line.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

glm::vec3 BarycentricInterpolation(Vertex iv1, Vertex iv2, Vertex iv3, glm::vec3 iPos);
//glm::vec3 BarycentricInterpolation(const glm::vec2 ip1,const glm::vec2 ip2,const glm::vec2 ip3, int x, int y);
Rasterizer::Rasterizer(const std::vector<Polygon>& polygons)
    : m_polygons(polygons)
{}

QImage Rasterizer::RenderScene()
{
    QImage result(512, 512, QImage::Format_RGB32);
    std::array<float, 512*512> zBuffer;

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
        std::cout << "poly size:" << m_polygons.size() << std::endl;
        std::cout << "tri size:" << tarPoly.m_tris.size() << std::endl;

        // Each loop draws a triangle.
        for(int triIndex = 0; triIndex < tarPoly.m_tris.size(); triIndex++)
        {
            Triangle tarTri = tarPoly.TriAt(triIndex);
            BoundingBox triBoundingBox(tarTri, tarPoly);

            float bigYBound = triBoundingBox.GetBiggestY();
            float smallYBound = triBoundingBox.GetSmallestY();
            float bigXBound = triBoundingBox.GetBiggestX();
            float smallXBound = triBoundingBox.GetSmallestX();

            // Get three points.
            struct Vertex v1 = tarPoly.VertAt(tarTri.m_indices[0]);
            struct Vertex v2 = tarPoly.VertAt(tarTri.m_indices[1]);
            struct Vertex v3 = tarPoly.VertAt(tarTri.m_indices[2]);

            glm::vec2 p1(v1.m_pos.x, v1.m_pos.y);
            glm::vec2 p2(v2.m_pos.x, v2.m_pos.y);
            glm::vec2 p3(v3.m_pos.x, v3.m_pos.y);

            // Construct three Edges.
            Line edges[3] = {Line(p1, p2), Line(p1, p3), Line(p2, p3)};

            // Drawing the triangle row by row. smallYBound
            for(int yRow = smallYBound; yRow <= bigYBound; yRow++)
            {
                // Get the two intercepted x values.
                //std::cout << "yRow:" << yRow << std::endl;
                float interceptedx[2];
                unsigned int ptr = 0;
                for(int i = 0; i < 3; i++)
                {
                    if (edges[i].YValueAndLineIntersection(yRow, &interceptedx[ptr]))
                    {
                        ptr++;
                        //std::cout << "We get here" << std::endl;
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
                    glm::vec3 rightVec = BarycentricInterpolation(v1, v2, v3, glm::vec3(iX, iY, iZ));

                    if ((iX < 0 || iX >= 512) || (iY < 0 || iY >= 512))
                    {
                        continue;
                    }
                    glm::vec3 tempColor = v1.m_color * rightVec[0] + v2.m_color * rightVec[1] + v3.m_color * rightVec[2];
                    float tempZ = v1.m_pos[2] * rightVec[0] + v2.m_pos[2] * rightVec[1] + v3.m_pos[2] * rightVec[2];
                    if(tempZ < zBuffer.at(iX + iY * 512))
                    {
                        zBuffer[iX + iY * 512] = tempZ;
                        result.setPixel(i, yRow, qRgb(tempColor.r, tempColor.g, tempColor.b));
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

glm::vec3 BarycentricInterpolation(Vertex iv1, Vertex iv2, Vertex iv3, glm::vec3 iPos)
{
    struct Vertex v1 = iv1;
    struct Vertex v2 = iv2;
    struct Vertex v3 = iv3;

    glm::vec3 p1(v1.m_pos.x, v1.m_pos.y, v1.m_pos.z);
    glm::vec3 p2(v2.m_pos.x, v2.m_pos.y, v2.m_pos.z);
    glm::vec3 p3(v3.m_pos.x, v3.m_pos.y, v3.m_pos.z);

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
