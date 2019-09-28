#include "polygon.h"
#include <glm/gtx/transform.hpp>

void Polygon::Triangulate()
{
    //TODO: Populate list of triangles
    // i equals to the index of every triangle.
    for(int i = 0; i < m_verts.size() - 2; i++)
    {
        Triangle temp;
        temp.m_indices[0] = 0;
        temp.m_indices[1] = i + 1;
        temp.m_indices[2] = i + 2;
        m_tris.push_back(temp);
    }
}

// Creates a polygon from the input list of vertex positions and colors
Polygon::Polygon(const QString& name, const std::vector<glm::vec4>& pos, const std::vector<glm::vec3>& col)
    : m_tris(), m_verts(), m_name(name), mp_texture(nullptr), mp_normalMap(nullptr)
{
    for(unsigned int i = 0; i < pos.size(); i++)
    {
        m_verts.push_back(Vertex(pos[i], col[i], glm::vec4(), glm::vec2()));
    }
    Triangulate();
}

// Creates a regular polygon with a number of sides indicated by the "sides" input integer.
// All of its vertices are of color "color", and the polygon is centered at "pos".
// It is rotated about its center by "rot" degrees, and is scaled from its center by "scale" units
Polygon::Polygon(const QString& name, int sides, glm::vec3 color, glm::vec4 pos, float rot, glm::vec4 scale)
    : m_tris(), m_verts(), m_name(name), mp_texture(nullptr), mp_normalMap(nullptr)
{
    glm::vec4 v(0.f, 1.f, 0.f, 1.f);
    float angle = 360.f / sides;
    for(int i = 0; i < sides; i++)
    {
        glm::vec4 vert_pos = glm::translate(glm::vec3(pos.x, pos.y, pos.z))
                           * glm::rotate(rot, glm::vec3(0.f, 0.f, 1.f))
                           * glm::scale(glm::vec3(scale.x, scale.y, scale.z))
                           * glm::rotate(i * angle, glm::vec3(0.f, 0.f, 1.f))
                           * v;
        m_verts.push_back(Vertex(vert_pos, color, glm::vec4(), glm::vec2()));
    }

    Triangulate();
}

Polygon::Polygon(const QString &name)
    : m_tris(), m_verts(), m_name(name), mp_texture(nullptr), mp_normalMap(nullptr)
{}

Polygon::Polygon()
    : m_tris(), m_verts(), m_name("Polygon"), mp_texture(nullptr), mp_normalMap(nullptr)
{}

Polygon::Polygon(const Polygon& p)
    : m_tris(p.m_tris), m_verts(p.m_verts), m_name(p.m_name), mp_texture(nullptr), mp_normalMap(nullptr)
{
    if(p.mp_texture != nullptr)
    {
        mp_texture = new QImage(*p.mp_texture);
    }
    if(p.mp_normalMap != nullptr)
    {
        mp_normalMap = new QImage(*p.mp_normalMap);
    }
}

Polygon::~Polygon()
{
    delete mp_texture;
}

void Polygon::SetTexture(QImage* i)
{
    mp_texture = i;
}

void Polygon::SetNormalMap(QImage* i)
{
    mp_normalMap = i;
}

void Polygon::AddTriangle(const Triangle& t)
{
    m_tris.push_back(t);
}

void Polygon::AddVertex(const Vertex& v)
{
    m_verts.push_back(v);
}

void Polygon::ClearTriangles()
{
    m_tris.clear();
}

Triangle& Polygon::TriAt(unsigned int i)
{
    return m_tris[i];
}

Triangle Polygon::TriAt(unsigned int i) const
{
    return m_tris[i];
}

Vertex &Polygon::VertAt(unsigned int i)
{
    return m_verts[i];
}

Vertex Polygon::VertAt(unsigned int i) const
{
    return m_verts[i];
}

glm::vec3 GetImageColor(const glm::vec2 &uv_coord, const QImage* const image)
{
    if(image)
    {
        int X = glm::min(image->width() * uv_coord.x, image->width() - 1.0f);
        int Y = glm::min(image->height() * (1.0f - uv_coord.y), image->height() - 1.0f);
        QColor color = image->pixel(X, Y);
        return glm::vec3(color.red(), color.green(), color.blue());
    }
    return glm::vec3(255.f, 255.f, 255.f);
}

BoundingBox::BoundingBox(const Triangle &iTri, const Polygon &iPoly)
{
    std::vector<glm::vec4> posArray;
    for(int i = 0; i < 3; i++)
    {
        posArray.push_back(iPoly.VertAt(iTri.m_indices[i]).m_pos);
    }


    // Find the bounding x, y values.
    // Init the x, y valuse.
    float biggestX, biggestY, smallestX, smallestY;
    std::vector<glm::vec4>::iterator ptr = posArray.begin();
    biggestX = ptr->x;
    smallestX = ptr->x;
    biggestY = ptr->y;
    smallestY = ptr->y;

    for(;ptr != posArray.end(); ptr++)
    {
        // Init.
        float currentX = ptr->x;
        float currentY = ptr->y;

        // For biggest X.
        if (currentX > biggestX)
        {
            biggestX = currentX;
        }

        // For smallest X.
        if (currentX < smallestX)
        {
            smallestX = currentX;
        }

        // For biggest Y.
        if (currentY > biggestY)
        {
            biggestY = currentY;
        }

        // For smallest Y.
        if(currentY < smallestY)
        {
            smallestY = currentY;
        }
    }

    // Using the bounding x, y values to construct the bounding points.
    upperLeft.x = smallestX;
    upperLeft.y = biggestY;

    bottomLeft.x = smallestX;
    bottomLeft.y = smallestY;

    upperRight.x = biggestX;
    upperRight.y = biggestY;

    bottomRight.x = biggestX;
    bottomRight.y = smallestY;
}

BoundingBox::BoundingBox(glm::vec4 a, glm::vec4 b, glm::vec4 c)
{
    std::vector<glm::vec4> posArray;
    posArray.push_back(a);
    posArray.push_back(b);
    posArray.push_back(c);

    // Find the bounding x, y values.
    // Init the x, y valuse.
    float biggestX, biggestY, smallestX, smallestY;
    std::vector<glm::vec4>::iterator ptr = posArray.begin();
    biggestX = ptr->x;
    smallestX = ptr->x;
    biggestY = ptr->y;
    smallestY = ptr->y;

    for(;ptr != posArray.end(); ptr++)
    {
        // Init.
        float currentX = ptr->x;
        float currentY = ptr->y;

        // For biggest X.
        if (currentX > biggestX)
        {
            biggestX = currentX;
        }

        // For smallest X.
        if (currentX < smallestX)
        {
            smallestX = currentX;
        }

        // For biggest Y.
        if (currentY > biggestY)
        {
            biggestY = currentY;
        }

        // For smallest Y.
        if(currentY < smallestY)
        {
            smallestY = currentY;
        }
    }

    // Using the bounding x, y values to construct the bounding points.
    upperLeft.x = smallestX;
    upperLeft.y = biggestY;

    bottomLeft.x = smallestX;
    bottomLeft.y = smallestY;

    upperRight.x = biggestX;
    upperRight.y = biggestY;

    bottomRight.x = biggestX;
    bottomRight.y = smallestY;
}

float BoundingBox::GetBiggestX()
{
    return upperRight.x;
}

float BoundingBox::GetBiggestY()
{
    return upperRight.y;
}

float BoundingBox::GetSmallestX()
{
    return bottomLeft.x;
}

float BoundingBox::GetSmallestY()
{
    return bottomLeft.y;
}

BoundingBox::~BoundingBox()
{}
