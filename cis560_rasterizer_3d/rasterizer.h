#pragma once
#include "camera.h"
#include <polygon.h>
#include <QImage>

class Rasterizer
{
private:
    //This is the set of Polygons loaded from a JSON scene file
    std::vector<Polygon> m_polygons;

public:
    Camera m_Camera;
    Rasterizer(const std::vector<Polygon>& polygons);
    QImage RenderScene();
    void ClearScene();
};
