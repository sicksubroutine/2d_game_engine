-- Level 1 definition
Level = {
------------------------------------------------------------------------------
-- Table to define list of assets to preload
------------------------------------------------------------------------------
    assets = {
        [0] =
        { type = "texture", id = "tilemap-image", file = "./assets/tilemaps/jungle.png", white_tex = false },
        { type = "texture", id = "chopper-image", file = "./assets/images/chopper-spritesheet.png", white_tex = true },
        { type = "texture", id = "tank-image",    file = "./assets/images/tank-tiger-up.png", white_tex = true },
        { type = "texture", id = "bullet-image",  file = "./assets/images/bullet.png", white_tex = false },
        { type = "font"   , id = "pico8-font-7",    file = "./assets/fonts/pico8.ttf", font_size = 7 },
        { type = "font"   , id = "pico8-font-10",   file = "./assets/fonts/pico8.ttf", font_size = 10 }
    },

------------------------------------------------------------------------------
-- Table to define the map config variables for the level
------------------------------------------------------------------------------

    tilemap = {
        map_file = "./assets/tilemaps/jungle.map",
        texture_asset_id = "tilemap-image",
        tile_size = 32,
        scale = 2.0
    },

------------------------------------------------------------------------------
-- Table to define entities and their components
------------------------------------------------------------------------------

    entities = {
        [0] =
        {
            -- Player
            tag = "player",
            group = "player",
            components = {
                transform = {
                    position = { x = 200, y = 110 },
                    scale = { x = 2.0, y = 2.0 },
                    rotation = 0.0, -- degrees
                },
                rigidbody = {
                    velocity = { x = 0.0, y = 0.0 }
                },
                sprite = {
                    texture_asset_id = "chopper-image",
                    width = 32,
                    height = 32,
                    layer = PLAYER_LAYER,
                    fixed = false,
                    src_rect_x = 0,
                    src_rect_y = 0
                },
                animation = {
                    num_frames = 2,
                    speed_rate = 15, -- frames per second
                    is_looping = true
                },
                boxcollider = {
                    width = 32,
                    height = 32,
                    offset = { x = 0, y = 0 }
                },
                health = {
                    current_health = 100,
                    max_health = 100,
                    is_god_mode = false
                },
                projectile_emitter = {
                    projectile_velocity = { x = 200, y = 200 },
                    projectile_duration = 10, -- seconds
                    repeat_frequency = 0, -- seconds
                    hit_damage = 10,
                    friendly = true
                },
                keyboard_controller = {
                    up_velocity = { x = 0, y = -80 },
                    right_velocity = { x = 80, y = 0 },
                    down_velocity = { x = 0, y = 80 },
                    left_velocity = { x = -80, y = 0 }
                },
                camera_follow = {
                    follow = true
                },
                text_label = {
                    text = "100", -- health bar
                    font_asset_id = "pico8-font-7",
                    color = GREEN,
                    is_fixed = false
                }
            }
        },
        {
            -- Tank
            group = "enemies",
            components = {
                transform = {
                    position = { x = 200, y = 497 },
                    scale = { x = 1.0, y = 1.0 },
                    rotation = 0.0, -- degrees
                },
                sprite = {
                    texture_asset_id = "tank-image",
                    width = 32,
                    height = 32,
                    layer = GROUND_LAYER
                },
                boxcollider = {
                    width = 25,
                    height = 18,
                    offset = { x = 0, y = 7 }
                },
                health = {
                    current_health = 100,
                    max_health = 100,
                    is_god_mode = false
                },
                projectile_emitter = {
                    projectile_velocity = { x = 100, y = 0 },
                    projectile_duration = 2, -- seconds
                    repeat_frequency = 1, -- seconds
                    hit_damage = 25,
                    friendly = false
                },
                text_label = {
                    text = "100", -- health bar
                    font_asset_id = "pico8-font-7",
                    color = GREEN,
                    is_fixed = false
                }
            }
        }
    },
}
