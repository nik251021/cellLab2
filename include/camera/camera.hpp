#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
private:
    glm::mat4 m_ProjectionMatrix;
    glm::mat4 m_ViewMatrix;
    glm::mat4 m_ViewProjectionMatrix;

    glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
    float m_Zoom = 1.0f;

public:
    Camera(float left, float right, float bottom, float top) {
        m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
        m_ViewMatrix = glm::mat4(1.0f);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    void setPosition(const glm::vec3& position) {
        m_Position = position;
        recalculateViewMatrix();
    }

    void setZoom(float zoom) {
        m_Zoom = zoom;
        recalculateViewMatrix();
    }

    const glm::mat4& getViewProjection() const { return m_ViewProjectionMatrix; }

private:
    void recalculateViewMatrix() {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) * 
                              glm::scale(glm::mat4(1.0f), glm::vec3(m_Zoom, m_Zoom, 1.0f));
        
        m_ViewMatrix = glm::inverse(transform);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }
};