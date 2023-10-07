
#include <fstream>
#include <sstream>
#include <iostream>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <sol/sol.hpp>

#include "./level_loader.h"
#include "./game.h"
#include "../ecs/ecs.h"
#include "../utils/utils.h"
#include "../logger/logger.h"

#include "../asset_store/asset_store.h"
#include "../components/transform_component.h"
#include "../components/rigid_body_component.h"
#include "../components/box_collider_component.h"
#include "../components/sprite_component.h"
#include "../components/animation_component.h"
#include "../components/keyboard_controlled_component.h"
#include "../components/camera_follow_component.h"
#include "../components/projectile_emitter_component.h"
#include "../components/health_component.h"
#include "../components/text_label_component.h"
#include "../components/script_component.h"
#include "../components/audio_component.h"


LevelLoader::LevelLoader() {
    Logger::Log("LevelLoader constructor called!");
}

LevelLoader::~LevelLoader() {
    Logger::Log("LevelLoader destructor called!");
}

void LevelLoader::LoadTileMap(const std::unique_ptr<Registry>& registry, std::string asset_id, std::string file_path, int tile_size, int tile_scale) {
    std::vector<std::vector<int>> tile_data;
    int scale = tile_size * tile_scale;

    std::ifstream file(file_path);
    std::string line;
    while (std::getline(file, line)) {
        std::vector<int> row;
        std::istringstream s(line);
        std::string field;
        while (getline(s, field, ',')) {
            row.push_back(std::stoi(field));
        }
        tile_data.push_back(row);
    }

    Game::map_width = tile_data[0].size() * scale;
    Game::map_height = tile_data.size() * scale;
    
    for (int y = 0; y < tile_data.size(); ++y) {
        for (int x = 0; x < tile_data[y].size(); ++x) {
            int tile_id = tile_data[y][x]; 

            SDL_Rect src_rect;
            int tile_row = tile_id / 10; 
            int tile_col = tile_id % 10;

            src_rect.x = tile_col * tile_size;
            src_rect.y = tile_row * tile_size;
            src_rect.w = tile_size;
            src_rect.h = tile_size;

            Entity tile_entity = registry->create_entity();
            tile_entity.Group("tiles");
            tile_entity.add_component<TransformComponent>(glm::vec2(x * (scale), y * (scale)), glm::vec2(tile_scale, tile_scale), 0.0);
            tile_entity.add_component<SpriteComponent>(asset_id, tile_size, tile_size, BACKGROUND_LAYER, src_rect.x, src_rect.y);
        }
    }
}

void LevelLoader::load_level(sol::state& lua, const std::unique_ptr<Registry>& registry, const std::unique_ptr<AssetStore>& asset_store, SDL_Renderer* renderer, int level_number) {

    std::string level_file = "./assets/scripts/Level" + std::to_string(level_number) + ".lua";
    sol::load_result script = lua.load_file(level_file);
    if (!script.valid()) {
        sol::error err = script;
        std::string error_msg = err.what();
        Logger::Err("Error loading level " + std::to_string(level_number) + ": " + error_msg);
        return;
    }
    
    lua.script_file(level_file);
    sol::table level = lua["Level"];
    Logger::Log("Loaded level " + std::to_string(level_number) + "!");
    

    // Load assets
    sol::table assets = level["assets"];

    int i = 0;
    while (true) {
        sol::optional<sol::table> asset = assets[i];
        if (asset == sol::nullopt) {
            break;
        }
        std::string asset_type = asset.value()["type"];
        std::string id = asset.value()["id"];
        std::string file = asset.value()["file"];
        if (asset_type == "texture") {
            asset_store->add_texture(renderer, id, file, asset.value()["white_tex"]);
        } else if (asset_type == "font") {
            asset_store->add_font(id, file, asset.value()["font_size"]);
        } else if (asset_type == "audio") {
            asset_store->add_audio(id, file);
        }
        i++;
    }

    // Load tilemap
    sol::table tilemap = level["tilemap"];
    std::string map_file = tilemap["map_file"];
    std::string texture_asset_id = tilemap["texture_asset_id"];
    int tile_size = tilemap["tile_size"];
    float scale = tilemap["scale"];
    LoadTileMap(registry, texture_asset_id, map_file, tile_size, scale);
    if (Game::verbose_logging) {
        Logger::Log("Loaded tilemap for level " + std::to_string(level_number)+ "!");
    }

    lua["map_width"] = Game::map_width;
    lua["map_height"] = Game::map_height;

    // Load entities
    sol::table entities = level["entities"];
    i = 0;
    while (true) {
        sol::optional<sol::table> entity = entities[i];
        if (entity == sol::nullopt) {
            break;
        }

        Entity new_entity = registry->create_entity();

        // Tag
        sol::optional<std::string> tag = entity.value()["tag"];
        if (tag != sol::nullopt) {
            if (Game::verbose_logging) {
                Logger::Log("Adding tag " + tag.value() + " to entity " + std::to_string(new_entity.get_id()));
            }
            new_entity.Tag(tag.value());
        }

        // Group
        sol::optional<std::string> group = entity.value()["group"];
        if (group != sol::nullopt) {
            if (Game::verbose_logging) {
                Logger::Log("Adding group " + group.value() + " to entity " + std::to_string(new_entity.get_id()));
            }
            new_entity.Group(group.value());
        }

        sol::optional<sol::table> components = entity.value()["components"];
        if (components != sol::nullopt) {
            // transform
            sol::optional<sol::table> transform = components.value()["transform"];
            if (transform != sol::nullopt) {
                
                new_entity.add_component<TransformComponent>(
                    glm::vec2(transform.value()["position"]["x"], transform.value()["position"]["y"]),
                    glm::vec2(transform.value()["scale"]["x"].get_or(1.0), transform.value()["scale"]["y"].get_or(1.0)),
                    transform.value()["rotation"].get_or(0.0)
                );
            }
            // rigidbody
            sol::optional<sol::table> rigid_body = components.value()["rigidbody"];
            if (rigid_body != sol::nullopt) {
                
                new_entity.add_component<RigidBodyComponent>(
                    glm::vec2(rigid_body.value()["velocity"]["x"].get_or(0.0), rigid_body.value()["velocity"]["y"].get_or(0.0))
                );
            }
            // sprite
            sol::optional<sol::table> sprite = components.value()["sprite"];
            if (sprite != sol::nullopt) {

                SpriteLayer layer = static_cast<SpriteLayer>(sprite.value()["layer"].get_or(0));
                
                new_entity.add_component<SpriteComponent>(
                    sprite.value()["texture_asset_id"],
                    sprite.value()["width"],
                    sprite.value()["height"],
                    layer,
                    sprite.value()["src_rect"]["x"].get_or(0),
                    sprite.value()["src_rect"]["y"].get_or(0),
                    sprite.value()["is_hidden"].get_or(true)
                );
            }
            // animation
            sol::optional<sol::table> animation = components.value()["animation"];
            if (animation != sol::nullopt) {
                
                new_entity.add_component<AnimationComponent>(
                    static_cast<int>(animation.value()["num_frames"]),
                    static_cast<int>(animation.value()["speed_rate"]),
                    animation.value()["is_looping"]
                );
            }
            // box_collider
            sol::optional<sol::table> box_collider = components.value()["boxcollider"];
            if (box_collider != sol::nullopt) {
                if (Game::verbose_logging) {
                    Logger::Log("Adding box collider component to entity " + std::to_string(new_entity.get_id()));
                }
                new_entity.add_component<BoxColliderComponent>(
                    static_cast<int>(box_collider.value()["width"]),
                    static_cast<int>(box_collider.value()["height"]),
                    glm::vec2(box_collider.value()["offset"]["x"].get_or(0.0), box_collider.value()["offset"]["y"].get_or(0.0))
                );
            }

            // camera_follow
            sol::optional<sol::table> camera_follow = components.value()["camera_follow"];
            if (camera_follow != sol::nullopt) {
                if (Game::verbose_logging) {
                    Logger::Log("Adding camera follow component to entity " + std::to_string(new_entity.get_id()));
                }
                new_entity.add_component<CameraFollowComponent>();
            }
            // health
            sol::optional<sol::table> health = components.value()["health"];
            if (health != sol::nullopt) {
                
                new_entity.add_component<HealthComponent>(
                    static_cast<int>(health.value()["current_health"]),
                    static_cast<int>(health.value()["max_health"]),
                    health.value()["is_god_mode"]
                );

                // add a health bar
                new_entity.add_component<TextLabelComponent>(
                    glm::vec2(0.0),
                    "100",
                    "pico8-font-7",
                    col.green,
                    false,
                    new_entity.get_id()
                );
            }
            // projectile_emitter
            sol::optional<sol::table> projectile_emitter = components.value()["projectile_emitter"];
            if (projectile_emitter != sol::nullopt) {
                
                new_entity.add_component<ProjectileEmitterComponent>(
                    glm::vec2(projectile_emitter.value()["projectile_velocity"]["x"], projectile_emitter.value()["projectile_velocity"]["y"]),
                    static_cast<int>(projectile_emitter.value()["repeat_frequency"].get_or(10) * 1000),
                    static_cast<int>(projectile_emitter.value()["projectile_duration"].get_or(10) * 1000), 
                    static_cast<int>(projectile_emitter.value()["hit_damage"].get_or(10)),
                    projectile_emitter.value()["friendly"]
                );
            }
            // keyboard_controller
            sol::optional<sol::table> keyboard_controller = components.value()["keyboard_controller"];
            if (keyboard_controller != sol::nullopt) {
            
                new_entity.add_component<KeyboardControlledComponent>(
                    glm::vec2(keyboard_controller.value()["up_velocity"]["x"], keyboard_controller.value()["up_velocity"]["y"]),
                    glm::vec2(keyboard_controller.value()["right_velocity"]["x"], keyboard_controller.value()["right_velocity"]["y"]),
                    glm::vec2(keyboard_controller.value()["down_velocity"]["x"], keyboard_controller.value()["down_velocity"]["y"]),
                    glm::vec2(keyboard_controller.value()["left_velocity"]["x"], keyboard_controller.value()["left_velocity"]["y"])
                );
            }

            // script
            sol::optional<sol::table> script_component = components.value()["on_update_script"];
            if (script_component != sol::nullopt) {
                sol::function func = script_component.value()[0];
                new_entity.add_component<ScriptComponent>(func);
            }

            // audio component
            sol::optional<sol::table> audio_component = components.value()["audio_source"];
            if (audio_component != sol::nullopt) {
                new_entity.add_component<AudioComponent>(
                    audio_component.value()["audio_asset_id"],
                    audio_component.value()["is_looping"],
                    audio_component.value()["volume"]
                );
            }
            
        }
        i++;  
            
    }

    // Entity blocking = registry->create_entity();
    // blocking.Group("obstacles");
    // blocking.add_component<TransformComponent>(glm::vec2(900.0, 500.0), glm::vec2(2.0, 2.0), 0.0);
    // blocking.add_component<BoxColliderComponent>(16, 200, glm::vec2(0.0, 0.0));

    // Entity blocking2 = registry->create_entity();
    // blocking2.Group("obstacles");
    // blocking2.add_component<TransformComponent>(glm::vec2(500.0, 500.0), glm::vec2(2.0, 2.0), 0.0);
    // blocking2.add_component<BoxColliderComponent>(16, 200, glm::vec2(0.0, 0.0));
}