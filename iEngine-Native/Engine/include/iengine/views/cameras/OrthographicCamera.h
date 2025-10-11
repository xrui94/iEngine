#ifndef IENGINE_ORTHOGRAPHIC_CAMERA_H
#define IENGINE_ORTHOGRAPHIC_CAMERA_H

#include "../../views/cameras/Camera.h"

namespace iengine {

    class OrthographicCamera : public Camera {
    public:
        OrthographicCamera(float left, float right, float bottom, float top, float near, float far);
        ~OrthographicCamera();

        float getLeft() const;
        float getRight() const;
        float getBottom() const;
        float getTop() const;
        float getNear() const;
        float getFar() const;

        void setLeft(float left);
        void setRight(float right);
        void setBottom(float bottom);
        void setTop(float top);
        void setNear(float near);
        void setFar(float far);

        void updateProjectionMatrix() override;

    private:
        float left_;
        float right_;
        float bottom_;
        float top_;
        float near_;
        float far_;
    };

} // namespace iengine

#endif // IENGINE_ORTHOGRAPHIC_CAMERA_H