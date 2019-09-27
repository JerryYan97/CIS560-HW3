#include "line.h"
#include <iostream>

Line::Line(glm::vec2 iEndPoint1, glm::vec2 iEndPoint2)
    : endPoint1(iEndPoint1), endPoint2(iEndPoint2)
{
    glm::vec2 leftPoint, rightPoint;

    if (endPoint1 == endPoint2)
    {
        // Two input points are the same points.
        slope = 0.f;
        state = 3;
    }
    else if (endPoint1[0] == endPoint2[0])
    {
        // This Line is vertical.
        slope = 0.f;
        state = 0;
    }
    else if (endPoint1[1] == endPoint2[1])
    {
        // This Line is horizontal.
        slope = 0.f;
        state = 1;
    }
    else
    {
        // This Line is a diagonal line.
        if (endPoint1[0] < endPoint2[0])
        {
            leftPoint = endPoint1;
            rightPoint = endPoint2;
        }
        else
        {
            leftPoint = endPoint2;
            rightPoint = endPoint1;
        }
        slope = (rightPoint[1] - leftPoint[1]) / (rightPoint[0] - leftPoint[0]);
        state = 2;
    }
}

bool Line::YValueAndLineIntersection(float iYValue, float *x)
{
    //std::cout << "State:" << state << std::endl;
    // Check the State.
    if (state == 0)
    {
        // Check whether this horizontal line intersects with this vertical Edge.
        float topY, bottomY;
        if (endPoint1[1] > endPoint2[1])
        {
            topY = endPoint1[1];
            bottomY = endPoint2[1];
        }
        else
        {
            topY = endPoint2[1];
            bottomY = endPoint1[1];
        }

        if (iYValue >= bottomY && iYValue <= topY)
        {
            *x = endPoint1[0];
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (state == 1)
    {
        // Ignore the horizontal Edge.
        return false;
    }
    else if (state == 3)
    {
        // Ignore the same endPoints edge.
        return false;
    }
    else if (state == 2)
    {
        // Check whether this horizontal line intersects with this diagonal Edge.
        // Check whether this horizontal line intersects with this vertical Edge.
        float topY, bottomY;
        if (endPoint1[1] > endPoint2[1])
        {
            topY = endPoint1[1];
            bottomY = endPoint2[1];
        }
        else
        {
            topY = endPoint2[1];
            bottomY = endPoint1[1];
        }

        if (iYValue >= bottomY && iYValue <= topY)
        {
            // Get the intersected x value.
            *x = ((iYValue - endPoint1[1]) / slope) + endPoint1[0];
            //std::cout << "slope:" << slope << std::endl;
            //std::cout << "x:" << *x << std::endl;
            //std::cout << "endPoint1[0]:" << endPoint1[0] << std::endl;
            //std::cout << "endPoint1[1]:" << endPoint1[1] << std::endl;
            //std::cout << "endPoint2[0]:" << endPoint2[0] << std::endl;
            //std::cout << "endPoint2[1]:" << endPoint2[1] << std::endl;
            //std::cout << std::endl;
            return true;
        }
        else
        {
            return false;
        }
    }
}
