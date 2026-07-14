#pragma once
#include <vector>

#include "instance_data.hpp"

class RenderBridge {
private:
    std::vector<InstanceData> m_Queue;

public:
    void drawCell(const glm::vec2& pos, float radius, float type, const glm::vec4& color) {
        m_Queue.emplace_back(pos, radius, type, color);
    }

    const std::vector<InstanceData>& getQueue() const { return m_Queue; }

    void clear() { m_Queue.clear(); }
};