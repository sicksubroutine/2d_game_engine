#ifndef DAMAGE_SYSTEM_H
#define DAMAGE_SYSTEM_H

#include "../ecs/ecs.h"
#include "../event_bus/event_bus.h"
#include "../events/collision_event.h"
#include "../components/box_collider_component.h"
#include "../components/projectile_component.h"
#include "../components/health_component.h"
#include "../components/sprite_component.h"

class DamageSystem: public System {
    public:
        DamageSystem() {
            require_component<BoxColliderComponent>();
        }

        void subscribe_to_events(std::unique_ptr<EventBus>& event_bus) {
            event_bus->subscribe_to_event<CollisionEvent>(this, &DamageSystem::on_collision);
        }

        void on_collision(CollisionEvent& event) {
            Entity a = event.a;
            Entity b = event.b;

            //Logger::Log("Collision event happened between entities " + std::to_string(a.get_id()) + " and " + std::to_string(b.get_id()) + ".");
            
            // check if a projectile hit a player
            if (a.BelongsToGroup("projectiles") && b.HasTag("player")) {
                on_projectile_hits_player(a, b);
            } else if (b.BelongsToGroup("projectiles") && a.HasTag("player")) {
                on_projectile_hits_player(b, a);
            }

            // check if a projectile hit an enemy
            if (a.BelongsToGroup("projectiles") && b.BelongsToGroup("enemies")) {
                on_projectile_hits_enemy(a, b);
            } else if (b.BelongsToGroup("projectiles") && a.BelongsToGroup("enemies")) {
                on_projectile_hits_enemy(b, a);
            }

        //    // check if player hit an enemy
        //     if (a.HasTag("player") && b.BelongsToGroup("enemies")) {

        //     } else if (b.HasTag("player") && a.BelongsToGroup("enemies")) {

        //     }

            // a debugging thing
            auto& a_collider = a.get_component<BoxColliderComponent>();
            auto& b_collider = b.get_component<BoxColliderComponent>();
            a_collider.is_colliding = true;
            b_collider.is_colliding = true;

        }

        void on_projectile_hits_player(Entity projectile, Entity player) {
            const auto projectile_component = projectile.get_component<ProjectileComponent>();
            auto& player_sprite = player.get_component<SpriteComponent>();
            if (projectile_component.is_friendly){
                return;
            }
            // kill the projectile
            projectile.Kill();

            // add to the hit flash
            player_sprite.hit_flash = 7;
            auto& health = player.get_component<HealthComponent>();
            health.health_percentage -= projectile_component.hit_percent_damage;

            //Logger::Log("Player " + std::to_string(player.get_id()) + " health is now " + std::to_string(health.health_percentage) + ".");
            if (health.health_percentage <= 0) {
                // kill the player when health is 0
                player.Kill();
            }        
        }

        void on_projectile_hits_enemy(Entity projectile, Entity enemy) {
            const auto projectile_component = projectile.get_component<ProjectileComponent>();

            if (!projectile_component.is_friendly){
                return;
            }

            auto& enemy_sprite = enemy.get_component<SpriteComponent>();

            // kill the projectile
            projectile.Kill();
            // add to the hit flash
            enemy_sprite.hit_flash = 7;
            auto& enemy_health = enemy.get_component<HealthComponent>();
            Logger::Log("Enemy health is " + std::to_string(enemy_health.health_percentage) + ".");
            enemy_health.health_percentage -= projectile_component.hit_percent_damage;

            if (enemy_health.health_percentage <= 0) {
                // kill the enemy when health is 0
                enemy.Kill();
            }
        }

};

#endif
