#include "camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

// GLM using a column-major matrix.
//matrix[col][row]
Camera::Camera()
{
    forwardDir = glm::vec4(0.0, 0.0, -1.0, 0.0);
    rightDir = glm::vec4(1.0, 0.0, 0.0, 0.0);
    upDir = glm::vec4(0.0, 1.0, 0.0, 0.0);
    vFOV = 45.f;
    pos = glm::vec4(0.0, 0.0, 10.f, 1.f);
    nearClipPlane = 0.01f;
    farClipPlane = 100.f;
    aspectRatio = 1.0f;

    xRadian = 0.f;
    yRadian = 0.f;
    zRadian = 0.f;
}

glm::mat4x4 Camera::GetViewMatrix()
{
    glm::mat4x4 orientationMatrix;
    glm::mat4x4 translationMatrix;

    // Construct OrientationMatrix.
    orientationMatrix[0][0] = rightDir.x;
    orientationMatrix[1][0] = rightDir.y;
    orientationMatrix[2][0] = rightDir.z;
    orientationMatrix[3][0] = 0.f;

    orientationMatrix[0][1] = upDir.x;
    orientationMatrix[1][1] = upDir.y;
    orientationMatrix[2][1] = upDir.z;
    orientationMatrix[3][1] = 0.f;

    orientationMatrix[0][2] = forwardDir.x;
    orientationMatrix[1][2] = forwardDir.y;
    orientationMatrix[2][2] = forwardDir.z;
    orientationMatrix[3][2] = 0.f;

    orientationMatrix[0][3] = 0.f;
    orientationMatrix[1][3] = 0.f;
    orientationMatrix[2][3] = 0.f;
    orientationMatrix[3][3] = 1.f;
    /****************************/

    // Construct TranslationMatrix.
    translationMatrix[0][0] = 1;
    translationMatrix[1][0] = 0;
    translationMatrix[2][0] = 0;
    translationMatrix[3][0] = -pos[0];

    translationMatrix[0][1] = 0;
    translationMatrix[1][1] = 1;
    translationMatrix[2][1] = 0;
    translationMatrix[3][1] = -pos[1];

    translationMatrix[0][2] = 0;
    translationMatrix[1][2] = 0;
    translationMatrix[2][2] = 1;
    translationMatrix[3][2] = -pos[2];

    translationMatrix[0][3] = 0;
    translationMatrix[1][3] = 0;
    translationMatrix[2][3] = 0;
    translationMatrix[3][3] = 1;
    /****************************/
    glm::mat4x4 check = orientationMatrix * translationMatrix;
    return check;
}

glm::mat4x4 Camera::GetPerspectiveProjMatrix()
{
    float f = farClipPlane;
    float n = nearClipPlane;
    float P = f / (f - n);
    float Q = - f * n / (f - n);
    float S = 1 / (glm::tan(vFOV / 2));
    float A = 512.f / 512.f;

    // Init the projMatrix.
    glm::mat4x4 projMatrix;
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            projMatrix[i][j] = 0.f;
        }
    }

    // Set values in the projMatrix.
    projMatrix[0][0] = S / A;
    projMatrix[1][1] = S;
    projMatrix[2][2] = P;
    projMatrix[2][3] = 1;
    projMatrix[3][2] = Q;

    return projMatrix;
}

void Camera::moveForwardAndBack(float dis)
{
    pos += dis * forwardDir;
}

void Camera::moveLeftAndRight(float dis)
{
    pos += dis * rightDir;
}

void Camera::moveUpAndDown(float dis)
{
    pos += dis * upDir;
}

// All the radian values rotating about different axies should be
// stored into the variables contained in the Camera class.
//
// Rotation is different from the translation, because rotation has meaning when
// it is exerted in the local coordinate.
//

void Camera::UpdateWorldDirection()
{
    // x-y-z.
    glm::mat4x4 rXMatrix;
    glm::mat4x4 rYMatrix;
    glm::mat4x4 rZMatrix;
    glm::mat4x4 rMatrix;

    rXMatrix = glm::rotate(glm::mat4(1.0), xRadian, glm::vec3(1, 0, 0));
    rZMatrix = glm::rotate(glm::mat4(1.0), zRadian, glm::vec3(0, 0, -1));
    rYMatrix = glm::rotate(glm::mat4(1.0), yRadian, glm::vec3(0, 1, 0));
    rMatrix = rXMatrix * rYMatrix * rZMatrix;

    rightDir = glm::vec4(1.0, 0.0, 0.0, 0.0) * rMatrix;
    forwardDir = glm::vec4(0.0, 0.0, -1.0, 0.0) * rMatrix;
    upDir = glm::vec4(0.0, 1.0, 0.0, 0.0) * rMatrix;
    /*
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            rXMatrix[i][j] = 0.f;
            rYMatrix[i][j] = 0.f;
            rZMatrix[i][j] = 0.f;
        }
    }

    // Set the elements in the matrix.
    rXMatrix[0][0] = 1;
    rXMatrix[1][1] = glm::cos(xRadian);
    rXMatrix[1][2] = -glm::sin(xRadian);
    rXMatrix[2][1] = glm::sin(xRadian);
    rXMatrix[2][2] = glm::cos(xRadian);
    rXMatrix[3][3] = 1;

    rYMatrix[0][0] = glm::cos(yRadian);
    rYMatrix[0][2] = glm::sin(yRadian);
    rYMatrix[1][1] = 1;
    rYMatrix[2][0] = -glm::sin(yRadian);
    rYMatrix[2][2] = glm::cos(yRadian);
    rYMatrix[3][3] = 1;

    rZMatrix[0][0] = glm::cos(zRadian);
    rZMatrix[0][1] = -glm::sin(zRadian);
    rZMatrix[1][0] = glm::sin(zRadian);
    rZMatrix[1][1] = glm::cos(zRadian);
    rZMatrix[2][2] = 1;
    rZMatrix[3][3] = 1;

    rMatrix = rXMatrix * rYMatrix * rZMatrix;

    // Change the axis dir.
    rightDir *= rMatrix;
    upDir *= rMatrix;
    forwardDir *= rMatrix;
    */
}

void Camera::rotateAroundXAxis(float radian)
{
    xRadian += radian;
    UpdateWorldDirection();
}

void Camera::rotateAroundYAxis(float radian)
{
    yRadian += radian;
    UpdateWorldDirection();
}

void Camera::rotateAroundZAxis(float radian)
{
    zRadian += radian;
    UpdateWorldDirection();
}
