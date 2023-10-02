#ifndef RENDER_GUI_SYSTEM_H
#define RENDER_GUI_SYSTEM_H

#include "../ecs/ecs.h"
#include <imgui/imgui.h>
#include <imgui/imgui_sdl.h>
#include "../logger/logger.h"
#include "../utils/utils.h"

class RenderGUISystem: public System {
    public:
        RenderGUISystem() = default;

        void Render(std::unique_ptr<Registry>& registry) {
            ImGui::NewFrame();


            if (ImGui::Begin("Spawn Enemies")) {
                static int enemy_type = 0;
                static std::string enemy_type_str;
                static glm::vec2 position = glm::vec2(0, 0);
                static float scale = 1.0f;
                ImGui::Text("Spawn Enemies Tool");
                ImGui::Separator();
                // position
                ImGui::Text("Enemy Position");
                ImGui::SliderFloat("X", &position.x, 0.0f, 1280.0f);
                ImGui::SliderFloat("Y", &position.y, 0.0f, 720.0f);
                ImGui::Separator();
                // scale
                ImGui::Text("Enemy Scale");
                ImGui::SliderFloat("Scale", &scale, 0.0f, 5.0f);
                ImGui::Separator();
                // health
                ImGui::Text("Enemy Health");
                static int enemy_health = 100;
                ImGui::SliderInt("Health", &enemy_health, 0, 1000);
                ImGui::Separator();
                // god mode for enemy?
                ImGui::Text("God Mode");
                static bool is_god_mode = false;
                ImGui::Checkbox("God Mode", &is_god_mode);
                ImGui::Separator();
                // enemy type
                ImGui::Text("Enemy Type");
                if (ImGui::BeginCombo("Enemy Type", enemy_type_str.c_str())) {
                    if (ImGui::Selectable("Tank", enemy_type == 0)) {
                        enemy_type = 0;
                        enemy_type_str = "Tank";
                    }
                    if (ImGui::Selectable("Truck", enemy_type == 1)) {
                        enemy_type = 1;
                        enemy_type_str = "Truck";
                    }
                    if (ImGui::Selectable("Helicopter", enemy_type == 2)) {
                        enemy_type = 2;
                        enemy_type_str = "Helicopter";
                    }
                    ImGui::EndCombo();
                }
                ImGui::Separator();
                if (ImGui::Button("Spawn Enemy")) {
                    SpawnEnemy(registry, position, scale, enemy_type_str, enemy_health, is_god_mode);
                }

                
            }
            ImGui::End();


            // player debugging
            auto player = registry->get_entity_by_tag("player");
            if (!player.BelongsToGroup("player")) {
                ImGui::Begin("Player Settings");
                ImGui::Text("Player died probably!");
                ImGui::Separator();
                ImGui::Text("Spawn Player");
                if (ImGui::Button("Spawn Player")) {
                    Entity chopper = registry->create_entity();
                    chopper.Tag("player");
                    chopper.Group("player");
                    chopper.add_component<TransformComponent>(glm::vec2(0.0, 0.0), glm::vec2(1.0, 1.0), 0.0);
                    chopper.add_component<RigidBodyComponent>(glm::vec2(0.0, 0.0));
                    chopper.add_component<SpriteComponent>("chopper-image", 32, 32, PLAYER_LAYER);
                    chopper.add_component<AnimationComponent>(2, 15, true);
                    chopper.add_component<BoxColliderComponent>(32, 32, glm::vec2(0.0));
                    chopper.add_component<KeyboardControlledComponent>(glm::vec2(0.0, -80.0),glm::vec2(80.0, 0.0), glm::vec2(0.0, 80.0), glm::vec2(-80.0, 0.0));
                    chopper.add_component<ProjectileEmitterComponent>(glm::vec2(150.0, 150.0), 0, 10000, 10, true);
                    chopper.add_component<CameraFollowComponent>();
                    chopper.add_component<HealthComponent>(100);
                    chopper.add_component<TextLabelComponent>(glm::vec2(0), "100", "pico8-font-7", col.green, false);
                }
                ImGui::End();    
            } else {
                if (ImGui::Begin("Player Settings")) {
                    auto& health = player.get_component<HealthComponent>();
                    static int player_health = 100;
                    ImGui::Text("God Mode");
                    static bool is_god_mode = false;
                    ImGui::Checkbox("God Mode", &is_god_mode);
                    health.is_god_mode = is_god_mode;
                    ImGui::Separator();
                    ImGui::Text("Player Health");
                    
                    ImGui::SliderInt("Health", &player_health, 10, 1000);
                    if (ImGui::Button("Set Health")) {
                        health.health_percentage = player_health;
                    }
                }
                ImGui::End();
            }
            
            

            ImGui::Render();
            ImGuiSDL::Render(ImGui::GetDrawData());
        }

        void SpawnEnemy(std::unique_ptr<Registry>& registry, glm::vec2 position, float scale, std::string enemy_str, int health, bool is_god_mode = false) {
            // convert string to the expected asset id
            if (enemy_str == "Tank") {
                enemy_str = "tank-image";
            } else if (enemy_str == "Truck") {
                enemy_str = "truck-image";
            } else if (enemy_str == "Helicopter") {
                enemy_str = "chopper-image";
            } else {
                enemy_str = "tank-image";
            }
            Entity entity = registry->create_entity();
            entity.Group("enemies");
            entity.add_component<TransformComponent>(position, glm::vec2(scale, scale), 0.0);
            
            entity.add_component<SpriteComponent>(enemy_str, 32, 32, GROUND_LAYER);
            entity.add_component<ProjectileEmitterComponent>(glm::vec2(0.0, -100.0), 1000, 5000, 10, false);
            entity.add_component<RigidBodyComponent>(glm::vec2(0.0, 0.0));
            entity.add_component<BoxColliderComponent>(32, 32, glm::vec2(0.0));
            if (enemy_str == "chopper-image") {
                entity.add_component<AnimationComponent>(2, 15, true);
            }
            entity.add_component<HealthComponent>(health, health, is_god_mode);
            entity.add_component<TextLabelComponent>(glm::vec2(0), "100", "pico8-font-7", col.green, false);
        }
};

#endif
