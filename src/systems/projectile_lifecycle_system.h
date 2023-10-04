#ifndef PROJECTILE_LIFECYCLE_SYSTEM_H
#define PROJECTILE_LIFECYCLE_SYSTEM_H

#include "../ecs/ecs.h"
#include "../components/projectile_component.h"
#include "../components/transform_component.h"
#include <SDL2/SDL.h>

class ProjectileLifecycleSystem: public System {
    public:
        ProjectileLifecycleSystem() {
            require_component<ProjectileComponent>();
            require_component<TransformComponent>();
        }

        void Update(SDL_Rect& camera) {
            
            for (auto entity: get_system_entities()) {
                auto projectile = entity.get_component<ProjectileComponent>();
                const auto transform = entity.get_component<TransformComponent>();

                // check if projectile has left the screen
                // taking the camera into account
                if (transform.position.x < 0 - camera.x || transform.position.x > camera.x + camera.w ||
                    transform.position.y < 0 - camera.y || transform.position.y > camera.y + camera.h) {
                    entity.Kill();
                }

                if (SDL_GetTicks() - projectile.start_time > projectile.duration) {
                    entity.Kill();
                }
            }
        }
};

#endif
