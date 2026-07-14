#include "rendering/renderer.hpp"
#include <glad/glad.h>

Renderer::Renderer() : m_maxInstances(10000) {
    // 1. Инициализируем шейдер
    m_shader = std::make_unique<Shader>("data/shaders/cell.vert", "data/shaders/cell.frag");

    // 2. Базовая геометрия (Квадрат)
    float vertices[] = {
        -1.0f, -1.0f,  // Лево низ
         1.0f, -1.0f,  // Право низ
         1.0f,  1.0f,  // Право верх
        -1.0f,  1.0f   // Лево верх
    };
    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    m_vao = std::make_unique<VertexArray>();
    m_vbo = std::make_unique<VertexBuffer>(vertices, sizeof(vertices));
    m_ebo = std::make_unique<IndexBuffer>(indices, 6);

    m_vao->addBuffer(*m_vbo);
    m_vao->setIndexBuffer(*m_ebo);

    // 3. Буфер для инстансинга (выделяем память с запасом)
    // Изначально создаем пустой буфер на m_maxInstances клеток
    m_instanceBuffer = std::make_unique<InstanceBuffer>(nullptr, m_maxInstances * sizeof(InstanceData));
    m_vao->addInstanceBuffer(*m_instanceBuffer);
}

void Renderer::beginScene(const Camera& camera) {
    glClearColor(0.1f, 0.1f, 0.11f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_shader->use();
    m_shader->setMat4("u_ViewProj", camera.getViewProjection());
}

void Renderer::drawCells(const std::vector<InstanceData>& cells) {
    if (cells.empty()) return;

    // Если клеток стало больше, чем вмещает буфер — пересоздаем его (защита от краша)
    if (cells.size() > m_maxInstances) {
        m_maxInstances = cells.size() * 2; // Увеличиваем с запасом
        m_instanceBuffer = std::make_unique<InstanceBuffer>(nullptr, m_maxInstances * sizeof(InstanceData));
        m_vao->addInstanceBuffer(*m_instanceBuffer); // Перепривязываем к VAO
    }

    // Обновляем данные в видеокарте
    m_instanceBuffer->updateData(cells.data(), cells.size() * sizeof(InstanceData));

    // Рисуем всё разом
    m_vao->bind();
    glDrawElementsInstanced(GL_TRIANGLES, m_ebo->getCount(), GL_UNSIGNED_INT, nullptr, cells.size());
}

void Renderer::endScene() {
    // В будущем здесь можно сбрасывать состояния OpenGL, если нужно
}