#include <physics/systems/genomeSystem.hpp>
#include <physics/world.hpp>
#include <physics/components.hpp>

void GenomeSystem::update(world& w, entt::registry& registry, GenomeRegistry& genomeRegistry, float dt) {
    std::vector<entt::entity> toSplit;
    registry.view<Mass, SplitComponent>().each([&](auto entity, auto& mass, auto& split) {
        if (mass.value >= split.splitMass) toSplit.push_back(entity);
    });

    for (auto e : toSplit) {
        if (registry.valid(e)) forceSplit(w, registry, genomeRegistry, e);
    }
}

void GenomeSystem::forceSplit(world& w, entt::registry& registry, GenomeRegistry& genomeRegistry, entt::entity parent) {
    if (!registry.all_of<GenomeComponent, Position, Mass, SplitComponent>(parent)) return;

    auto& gen = registry.get<GenomeComponent>(parent);
    auto& pos = registry.get<Position>(parent);
    auto& mass = registry.get<Mass>(parent);
    auto& split = registry.get<SplitComponent>(parent);

    const auto& mod = genomeRegistry.getModule(gen.genomeName, gen.currentModuleIndex);
    if (mod.childs.empty()) return;

    float newMass = mass.value / 2.0f;
    std::vector<entt::entity> children;

    float angleRad = glm::radians(mod.params.count("split_angle") ? mod.params.at("split_angle") : 0.0f);
    glm::vec2 baseDir = glm::vec2(cos(angleRad), sin(angleRad));

    for (auto const& [key, nextModuleIndex] : mod.childs) {
        glm::vec2 direction = (key == "child1") ? baseDir : -baseDir;
        glm::vec2 offset = direction * 7.5f; 

        auto child = w.spawnCellFromModule(gen.genomeName, nextModuleIndex, pos.value + offset);
        if (child != entt::null) {
            registry.get<Mass>(child).value = newMass;
            children.push_back(child);
        }
    }

    if (!children.empty()) transferAdhesions(w, registry, parent, children);

    if (split.makeAdhesin && children.size() >= 2) {
        w.makeAdhesin(children[0], children[1], 10.0f, 30.0f, 50.0f);
    }

    registry.destroy(parent);
}

void GenomeSystem::transferAdhesions(world& w, entt::registry& registry, entt::entity oldParent, const std::vector<entt::entity>& children) {
    auto view = registry.view<Adhesion>();
    std::vector<Adhesion> newAdhesions;

    for (auto entity : view) {
        auto& adj = view.get<Adhesion>(entity);
        if (adj.cellA == oldParent || adj.cellB == oldParent) {
            entt::entity partner = (adj.cellA == oldParent) ? adj.cellB : adj.cellA;
            for (auto child : children) {
                newAdhesions.push_back({child, partner, adj.restLength, adj.maxLength, adj.strength});
            }
            registry.destroy(entity);
        }
    }
    for (auto& adj : newAdhesions) w.makeAdhesin(adj.cellA, adj.cellB, adj.restLength, adj.maxLength, adj.strength);
}