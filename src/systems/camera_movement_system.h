#ifndef CAMERA_MOVEMENT_SYSTEM_H
#define CAMERA_MOVEMENT_SYSTEM_H

#include "../ecs/ecs.h"
#include "../components/camera_follow_component.h"
#include "../components/transform_component.h"
#include <SDL2/SDL.h>

class CameraMovementSystem: public System {
    public:
        CameraMovementSystem() {
            require_component<CameraFollowComponent>();
            require_component<TransformComponent>();
        }

        void Update(SDL_Rect& camera, int map_width, int map_height) {
            for (auto entity: get_system_entities()) {
                auto transform = entity.get_component<TransformComponent>();


                if (transform.position.x + (camera.w / 2) < map_width) {
                    camera.x = transform.position.x - (camera.w / 2);
                }
                if (transform.position.y + (camera.h / 2) < map_height) {
                    camera.y = transform.position.y - (camera.h / 2);
                }
                camera.x = camera.x < 0 ? 0 : camera.x;
                camera.y = camera.y < 0 ? 0 : camera.y;
                camera.x = camera.x > camera.w ? camera.w : camera.x;
                camera.y = camera.y > camera.h ? camera.h : camera.y;

                //Logger::Log("Camera x: " + std::to_string(camera.x) + " Camera y: " + std::to_string(camera.y));
                
            }
        }
};

#endif 
