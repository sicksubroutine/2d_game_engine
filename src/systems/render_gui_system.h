#ifndef RENDER_GUI_SYSTEM_H
#define RENDER_GUI_SYSTEM_H

#include "../ecs/ecs.h"
#include <imgui/imgui.h>
#include <imgui/imgui_sdl.h>
#include "../logger/logger.h"
#include "../utils/utils.h"
#include <SDL2/SDL.h>

class RenderGUISystem: public System {
    public:
        RenderGUISystem() = default;

        void Render(std::unique_ptr<Registry>& registry, SDL_Rect& camera, int map_width, int map_height) {
            ImGui::NewFrame();
            static bool is_debug = true;
            static bool is_console_log = true;
            static bool enemy_spawn_tool = false;
            static bool player_settings = true;

            if (ImGui::BeginMainMenuBar()) {
                
               if (ImGui::BeginMenu("Debug")) {
                    if (ImGui::MenuItem("Toggle Debug Panel", "Cmd+D", &is_debug, true)) {
                        
                    }
                    if (ImGui::MenuItem("Toggle Console Log", "Cmd+L", &is_console_log, true)) {
                        
                    }
                    if (ImGui::MenuItem("Toggle Enemy Spawn Tool", "Cmd+E", &enemy_spawn_tool, true)) {
                        
                    }
                    if (ImGui::MenuItem("Toggle Player Settings", "Cmd+P", &player_settings, true)) {
                        
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }

            if (is_debug) {
                if (ImGui::Begin("Debug Panel")) {
                    ImGui::Text("FPS: %d", Utils::GetFPS());
                    ImGui::Separator();
                    // get mouse position from imgui
                    ImGuiIO& io = ImGui::GetIO();
                    ImGui::Text("Mouse Position: (%d, %d)", static_cast<int>(io.MousePos.x), static_cast<int>(io.MousePos.y));
                    ImGui::Separator();
                    ImGui::Text("Camera Position: (%d, %d)", camera.x, camera.y);
                    ImGui::Separator();
                    // map width and height
                    ImGui::Text("Map Width: %d", map_width);
                    ImGui::Text("Map Height: %d", map_height);
                }
                ImGui::End();
            }
            ImGui::SetNextWindowBgAlpha(0.5);
            if (is_console_log) {
                if (ImGui::Begin("Console Log")) {
                    ImGui::BeginChild("Static Section", ImVec2(0, 40), false, ImGuiWindowFlags_None);
                    static bool auto_scroll = true;
                    // checkbox button for auto scroll
                    if (ImGui::Checkbox("Auto Scroll", &auto_scroll)) {
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Clear Log")) {
                        Logger::clear_logs();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Test Log")) {
                        Logger::Log("This is a test log");
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Test Error")) {
                        Logger::Err("This is a test error");
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Test Warning")) {
                        Logger::Warn("This is a test warning");
                    }
                    ImGui::EndChild();

                    ImGui::BeginChild("Scrolling Region", ImVec2(0,0), false, ImGuiWindowFlags_HorizontalScrollbar);
                    auto logs = Logger::get_logs();
                    for (auto log : logs) {
                        if (log.type == LOG_ERROR) {
                            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                        } else if (log.type == LOG_INFO) {
                            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
                        } else if (log.type == LOG_WARNING) {
                            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
                        }
                        std::string msg = log.message;
                        ImGui::Text("%s", msg.c_str());
                        ImGui::PopStyleColor();
                    }

                    if (auto_scroll) {
                        ImGui::SetScrollHereY(1.0f);
                    }

                    ImGui::EndChild();
                }

                ImGui::End();
            }
            


            if (enemy_spawn_tool) {
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
                    static int enemy_max_health = 100;
                    ImGui::SliderInt("Health", &enemy_health, 0, 1000);
                    ImGui::SliderInt("Max Health", &enemy_max_health, 0, 1000);
                    ImGui::Separator();
                    // god mode for enemy?
                    ImGui::Text("God Mode");
                    static bool is_god_mode = false;
                    ImGui::Checkbox("God Mode", &is_god_mode);
                    ImGui::Separator();
                    // direction and speed of enemy projectiles
                    ImGui::Text("Projectile Direction and Speed");
                    static glm::vec2 projectile_direction = glm::vec2(0.0, -100.0);
                    static int projectile_repeat_frequency = 5000; // in milliseconds
                    static int projectile_damage = 10;
                    static int projectile_speed = 100;
                    ImGui::SliderFloat("Proj Vel x", &projectile_direction.x, -1000.0f, 1000.0f);
                    ImGui::SliderFloat("Proj Vel y", &projectile_direction.y, -1000.0f, 1000.0f);
                    ImGui::SliderInt("Speed", &projectile_speed, 0, 1000);
                    // a button that will set the projectile direction to the player's direction
                    if (ImGui::Button("Set Proj Dir to Player Dir")) {
                        projectile_direction = get_player_angle(registry, position);
                        // multiply by the projectile speed
                        projectile_direction *= projectile_speed;
                    }
                    ImGui::SliderInt("Repeat Time", &projectile_repeat_frequency, 0, 10000);
                    ImGui::SliderInt("Damage", &projectile_damage, 0, 100);


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
                        SpawnEnemy(
                            registry, 
                            position, 
                            scale, 
                            enemy_type_str, 
                            enemy_health, 
                            enemy_max_health,
                            is_god_mode, 
                            projectile_direction,
                            projectile_repeat_frequency, 
                            projectile_damage);
                    }

                    
                }
                ImGui::End();
            }
            
            if (player_settings) {
                auto player_group = registry->get_entities_by_group("player");
                if (player_group.empty()) {
                    ImGui::Begin("Player Settings");
                    ImGui::Text("Player died probably!");
                    ImGui::Separator();
                    ImGui::Text("Spawn Player");
                    if (ImGui::Button("Spawn Player")) {
                        Entity chopper = registry->create_entity();
                        chopper.Tag("player");
                        chopper.Group("player");
                        chopper.add_component<TransformComponent>(glm::vec2(0.0, 100.0), glm::vec2(2.0, 2.0), 0.0);
                        chopper.add_component<RigidBodyComponent>(glm::vec2(0.0, 0.0));
                        chopper.add_component<SpriteComponent>("chopper-texture", 32, 32, PLAYER_LAYER);
                        chopper.add_component<AnimationComponent>(2, 15, true);
                        chopper.add_component<BoxColliderComponent>(32, 32, glm::vec2(0.0));
                        chopper.add_component<KeyboardControlledComponent>(glm::vec2(0.0, -80.0),glm::vec2(80.0, 0.0), glm::vec2(0.0, 80.0), glm::vec2(-80.0, 0.0));
                        chopper.add_component<ProjectileEmitterComponent>(glm::vec2(150.0, 150.0), 0, 10000, 10, true);
                        chopper.add_component<CameraFollowComponent>();
                        chopper.add_component<HealthComponent>(100, 100, false);
                        chopper.add_component<TextLabelComponent>(glm::vec2(0), "100", "pico8-font-7", col.green, false);
                    }
                    ImGui::End();
                } 
                for (auto entity: player_group) {
                    if (entity.HasTag("player")) {
                        if (ImGui::Begin("Player Settings")) {
                            auto& health = entity.get_component<HealthComponent>();
                            auto& transform = entity.get_component<TransformComponent>();
                            auto sprite = entity.get_component<SpriteComponent>();
                            static int player_health = 100;
                            static int player_max_health = 100;
                            ImGui::Text("God Mode");
                            static bool is_god_mode = false;
                            ImGui::Checkbox("God Mode", &is_god_mode);
                            health.is_god_mode = is_god_mode;
                            ImGui::Separator();
                            ImGui::Text("Player Health: %d", health.current_health);
                            ImGui::Text("Player Max Health: %d", health.max_health);
                            ImGui::Separator();
                            ImGui::SliderInt("Health", &player_health, 10, 1000);
                            ImGui::SliderInt("Max Health", &player_max_health, 10, 1000);
                            if (ImGui::Button("Set Health")) {
                                health.current_health = player_health;
                                health.max_health = player_max_health;
                            }
                            ImGui::Separator();
                            float player_pos_x = transform.position.x;
                            float player_pos_y = transform.position.y;
                            ImGui::Text("Player Position: (%f, %f)", player_pos_x, player_pos_y);
                            ImGui::Separator();
                            // set player position
                            static glm::vec2 player_position = glm::vec2(0.0, 0.0);
                            ImGui::SliderFloat("X", &player_position.x, 0.0f, static_cast<float>(map_width - sprite.width * transform.scale.x));
                            ImGui::SliderFloat("Y", &player_position.y, 0.0f, static_cast<float>(map_height - sprite.height * transform.scale.y));
                            if (ImGui::Button("Set Player Position")) {
                                transform.position = player_position;
                                camera.x = transform.position.x - (camera.w / 2);
                                camera.y = transform.position.y - (camera.h / 2);
                                camera.x = camera.x < 0 ? 0 : camera.x;
                                camera.y = camera.y < 0 ? 0 : camera.y;
                                camera.x = camera.x > camera.w ? camera.w : camera.x;
                                camera.y = camera.y > camera.h ? camera.h : camera.y;
                            }
                        }
                        ImGui::End();
                    }
                }
            }

            ImGui::Render();
            ImGuiSDL::Render(ImGui::GetDrawData());
        }

        glm::vec2 get_player_angle(std::unique_ptr<Registry>& registry, glm::vec2 enemy_position) {
            auto player = registry->get_entity_by_tag("player");
            if (player.BelongsToGroup("player")) {
                auto& player_transform = player.get_component<TransformComponent>();
                glm::vec2 player_position = player_transform.position;
                glm::vec2 player_direction = glm::normalize(player_position - enemy_position);
                return player_direction;
            } else {
                return glm::vec2(-1.0, -1.0);
            }
        }

        void SpawnEnemy(
            std::unique_ptr<Registry>& registry, 
            glm::vec2 position, 
            float scale, 
            std::string enemy_str, 
            int health, 
            int max_health,
            bool is_god_mode = false,
            glm::vec2 projectile_direction = glm::vec2(0.0, -100.0),
            int projectile_repeat_frequency = 5000,
            int projectile_damage = 10
        ) {
            if (enemy_str == "Tank") {
                enemy_str = "sam-tank-left-texture";
            } else if (enemy_str == "Truck") {
                enemy_str = "truck-ford-left-texture";
            } else if (enemy_str == "Helicopter") {
                enemy_str = "chopper-texture";
            } else {
                enemy_str = "sam-tank-left-texture";
            }
            Entity entity = registry->create_entity();
            entity.Group("enemies");
            entity.add_component<TransformComponent>(position, glm::vec2(scale, scale), 0.0);
            
            entity.add_component<SpriteComponent>(enemy_str, 32, 32, GROUND_LAYER);
            entity.add_component<ProjectileEmitterComponent>(projectile_direction, projectile_repeat_frequency, 5000, projectile_damage, false);
            entity.add_component<RigidBodyComponent>(glm::vec2(0.0, 0.0));
            entity.add_component<BoxColliderComponent>(32, 32, glm::vec2(0.0));
            if (enemy_str == "chopper-image") {
                entity.add_component<AnimationComponent>(2, 15, true);
            }
            entity.add_component<HealthComponent>(health, max_health, is_god_mode);
            entity.add_component<TextLabelComponent>(glm::vec2(0), "100", "pico8-font-7", col.green, false);
        }
};

#endif
