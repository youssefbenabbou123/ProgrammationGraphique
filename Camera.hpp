#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "GL/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

struct CameraKeyframe {
    glm::vec3 position;
    glm::vec3 target;
    float fov;

    CameraKeyframe(const glm::vec3& pos, const glm::vec3& tgt, float fieldOfView)
        : position(pos), target(tgt), fov(fieldOfView) {}
};

class Camera {

    public:

        glm::mat4 getViewMatrix() const;

        virtual void SetPosition(const glm::vec3& position){}
        virtual void rotate(float yaw,float pitch){}
        virtual void move(const glm::vec3& offsetPos){}

        const glm::vec3& getLook() const;
        const glm::vec3& getRight() const;
        const glm::vec3& getUp() const;
        const glm::vec3& getPosition() const;

    const glm::vec3& getTargetPos() const { return mTargetPos; }

    void setTargetPos(const glm::vec3& targetPos) { mTargetPos = targetPos; }

        float getFOV() const { return mFoV;}
        void setFOV(float fov){ mFoV = fov;}

    protected:

        Camera();

        virtual void updateCameraVectors(){}

        glm::vec3 mPosition;
        glm::vec3 mTargetPos;
        glm::vec3 mUp;
        glm::vec3 mLook;
        glm::vec3 mRight;
        const glm::vec3 WORLD_UP;

        float mYaw;
        float mPitch;

        float mFoV;

};

class FPSCamera : public Camera {
    public:

        FPSCamera(glm::vec3 position = glm::vec3(0.0f,0.0f,0.0f),float yaw = glm::pi<float>(),float pitch = 0.0f);

        virtual void setPosition(const glm::vec3& position);
        virtual void rotate(float yaw, float pitch);
        virtual void move(const glm::vec3& offsetPos);

    private:

        void updateCameraVectors();

};

class OrbitCamera :public Camera{

    public:

        OrbitCamera();

        virtual void rotate(float yaw,float pitch);

        void setlookAt(const glm::vec3& target);
        void setRadius(float radius);

    private:

        void updateCameraVectors();

        float mRadius;

};

#endif
