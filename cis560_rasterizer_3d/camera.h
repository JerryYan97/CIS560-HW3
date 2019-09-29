#ifndef CAMERA_H
#define CAMERA_H
#include<glm/glm.hpp>
#include<vector>

class Camera
{
public:
    Camera();
    glm::mat4x4 GetViewMatrix();
    glm::mat4x4 GetPerspectiveProjMatrix();

    void moveForwardAndBack(float dis);
    void moveLeftAndRight(float dis);
    void moveUpAndDown(float dis);

    void rotateAroundXAxis(float radian);
    void rotateAroundYAxis(float radian);
    void rotateAroundZAxis(float radian);

    glm::vec4 forwardDir; // The camera's "forward" direction, i.e. its Z axis. Default value of <0, 0, -1, 0>.
    glm::vec4 rightDir; // The camera's "right" direction, i.e. its X axis. Default value of <1, 0, 0, 0>.
    glm::vec4 upDir; // The camera's "up" direction, i.e. its Y axis. Default value of <0, 1, 0, 0>.
private:
    void UpdateWorldDirection();
    // Here are the camera direction axis in the world space.


    // Here are the radians rotated the local-axis of the camera.
    // The sequence is suppoed to be x-y-z.
    float xRadian;
    float yRadian;
    float zRadian;

    float vFOV; // Camera's vertical field of view.
    glm::vec4 pos; // The camera's position in world space. Default value of <0, 0, 10, 1>.
    float nearClipPlane; // The camera's near clip plane. Default value of 0.01.
    float farClipPlane; // The camera's far clip plane. Default value of 100.0.
    float aspectRatio; //  the camera's aspect ratio. Default value of 1.0.
};

#endif // CAMERA_H
