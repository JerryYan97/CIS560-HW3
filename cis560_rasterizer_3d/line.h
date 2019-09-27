#ifndef LINE_H
#define LINE_H

#include<glm/glm.hpp>
#include<vector>

class Line
{
public:
    Line(glm::vec2 iEndPoint1, glm::vec2 iEndPoint2);
    bool YValueAndLineIntersection(float iYValue, float* x);
private:
    glm::vec2 endPoint1;
    glm::vec2 endPoint2;
    float slope;
    unsigned int state; // 0 - vertical, 1 - horizontal, 2 - diagonal, 3 - same endpoints.
};

#endif // LINE_H
