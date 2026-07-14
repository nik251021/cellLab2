#pragma once
#include <glm/glm.hpp>

class Camera {
public:
    Camera(float width, float height);

    glm::mat4 getViewProjection() const;

    void setPosition(glm::vec2 pos);
    void setZoom(float zoom);

private:
    void updateProjection();

    glm::vec2 m_position;
    float m_zoom;
    float m_width, m_height;
    glm::mat4 m_projection;
};