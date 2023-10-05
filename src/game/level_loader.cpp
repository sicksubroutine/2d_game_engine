
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
        }
        i++;
    }

    // Load tilemap
    sol::table tilemap = level["tilemap"];
    std::string map_file = tilemap["map_file"];
    std::string texture_asset_id = tilemap["texture_asset_id"];
    int tile_size = tilemap["tile_size"];
    double scale = tilemap["scale"];
    LoadTileMap(registry, texture_asset_id, map_file, tile_size, scale);
    Logger::Log("Loaded tilemap for level " + std::to_string(level_number)+ "!");

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
            new_entity.Tag(tag.value());
        }

        // Group
        sol::optional<std::string> group = entity.value()["group"];
        if (group != sol::nullopt) {
            new_entity.Group(group.value());
        }
    }

    // // Add fonts to the asset store
    // asset_store->add_font("charriot-font", "./assets/fonts/charriot.ttf", 16);
    // asset_store->add_font("pico8-font-10", "./assets/fonts/pico8.ttf", 10);
    // asset_store->add_font("pico8-font-7", "./assets/fonts/pico8.ttf", 7);

    // // Adding assets to the asset store
    // asset_store->add_texture(renderer, "tilemap", "./assets/tilemaps/jungle.png", false);
    // asset_store->add_texture(renderer, "tank-image", "./assets/images/tank-panther-right.png", true);
    // asset_store->add_texture(renderer, "tank-left-image", "./assets/images/tank-panther-left.png", true);
    // asset_store->add_texture(renderer, "truck-image", "./assets/images/truck-ford-right.png", true);
    // asset_store->add_texture(renderer, "chopper-image", "./assets/images/chopper-spritesheet.png", true);
    // asset_store->add_texture(renderer, "radar-image", "./assets/images/radar.png", true);
    // asset_store->add_texture(renderer, "bullet-image", "./assets/images/bullet.png", true);
    // asset_store->add_texture(renderer, "tree-image", "./assets/images/tree.png", false);
    
    // // loading the tilemap
    // LoadTileMap(registry, "tilemap", "./assets/tilemaps/jungle.map", 32, 2.0);
    // Logger::Log("Loaded tilemap for level " + std::to_string(level_number)+ "!");
    // Entity label = registry->create_entity();
    // label.Group("GUI");
    // label.add_component<TextLabelComponent>(glm::vec2(Game::window_width/2-40, 10), "CHOPPER 1.0", "pico8-font-10", col.green, true);

    // // create the radar entity
    // Entity radar = registry->create_entity();
    // radar.Group("GUI");
    // radar.add_component<TransformComponent>(glm::vec2(Game::window_width-74, 10.0), glm::vec2(1.0, 1.0), 0.0);
    // radar.add_component<RigidBodyComponent>(glm::vec2(0.0, 0.0));
    // radar.add_component<SpriteComponent>("radar-image", 64, 64, GUI_LAYER);
    // radar.add_component<AnimationComponent>(8, 5, true);

    // //create some entities
    // Entity chopper = registry->create_entity();
    // chopper.Tag("player");
    // chopper.Group("player");
    // chopper.add_component<TransformComponent>(glm::vec2(0.0, 100.0), glm::vec2(2.0, 2.0), 0.0);
    // chopper.add_component<RigidBodyComponent>(glm::vec2(0.0, 0.0));
    // chopper.add_component<SpriteComponent>("chopper-image", 32, 32, PLAYER_LAYER);
    // chopper.add_component<AnimationComponent>(2, 15, true);
    // chopper.add_component<BoxColliderComponent>(24, 24, glm::vec2(4.0, 4.0));
    // chopper.add_component<KeyboardControlledComponent>(glm::vec2(0.0, -80.0),glm::vec2(80.0, 0.0), glm::vec2(0.0, 80.0), glm::vec2(-80.0, 0.0));
    // chopper.add_component<ProjectileEmitterComponent>(glm::vec2(150.0, 150.0), 0, 10000, 10, true);
    // chopper.add_component<CameraFollowComponent>();
    // chopper.add_component<HealthComponent>(100, 100, false);
    // chopper.add_component<TextLabelComponent>(glm::vec2(0), "100", "pico8-font-7", col.green, false);

    // Entity tank = registry->create_entity();
    // tank.Group("enemies");
    // tank.add_component<TransformComponent>(glm::vec2(525.0, 600.0), glm::vec2(2.0, 2.0), 0.0);
    // tank.add_component<RigidBodyComponent>(glm::vec2(50.0, 0.0));
    // tank.add_component<SpriteComponent>("tank-image", 32, 32, GROUND_LAYER);
    // tank.add_component<BoxColliderComponent>(22, 22, glm::vec2(10.0, 10.0));
    // tank.add_component<ProjectileEmitterComponent>(glm::vec2(0.0, -200.0), 750, 5000, 25, false);
    // tank.add_component<HealthComponent>(100, 100, false);
    // tank.add_component<TextLabelComponent>(glm::vec2(0), "100", "pico8-font-7", col.green, false);

    // Entity blocking = registry->create_entity();
    // blocking.Group("obstacles");
    // blocking.add_component<TransformComponent>(glm::vec2(900.0, 500.0), glm::vec2(2.0, 2.0), 0.0);
    // blocking.add_component<BoxColliderComponent>(16, 200, glm::vec2(0.0, 0.0));

    // Entity blocking2 = registry->create_entity();
    // blocking2.Group("obstacles");
    // blocking2.add_component<TransformComponent>(glm::vec2(500.0, 500.0), glm::vec2(2.0, 2.0), 0.0);
    // blocking2.add_component<BoxColliderComponent>(16, 200, glm::vec2(0.0, 0.0));

    // Entity truck = registry->create_entity();
    // truck.Group("enemies");
    // truck.add_component<TransformComponent>(glm::vec2(100.0, 475.0), glm::vec2(2.0, 2.0), 0.0);
    // truck.add_component<RigidBodyComponent>(glm::vec2(0.0, 0.0));
    // truck.add_component<SpriteComponent>("truck-image", 32, 32, GROUND_LAYER);
    // truck.add_component<BoxColliderComponent>(22, 22, glm::vec2(10.0, 10.0));
    // truck.add_component<ProjectileEmitterComponent>(glm::vec2(0.0, -100.0), 1000, 5000, 10, false);
    // truck.add_component<HealthComponent>(50, 100, false);
    // truck.add_component<TextLabelComponent>(glm::vec2(0), "100", "pico8-font-7", col.green, false);
}