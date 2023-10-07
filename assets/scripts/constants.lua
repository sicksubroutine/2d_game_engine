-- game config
config = {
    title = "My Game Engine",
    full_screen = false,
    target_fps=60 ,
    debug = false,
    verbose_logging = false,
    debug_to_console = false,
    resolution = {
        window_width = 1280,
        window_height = 720
    },
}

-- sprite layers
BACKGROUND_LAYER = 0
DECORATION_LAYER = 1
GROUND_LAYER = 2
AIR_LAYER = 3
BULLET_LAYER = 4
PLAYER_LAYER = 5
GUI_LAYER = 6

-- color constants
WHITE = { r = 255, g = 255, b = 255 }
BLACK = { r = 0, g = 0, b = 0 }
RED = { r = 255, g = 0, b = 0 }
GREEN = { r = 0, g = 255, b = 0 }
BLUE = { r = 0, g = 0, b = 255 }
YELLOW = { r = 255, g = 255, b = 0 }
MAGENTA = { r = 255, g = 0, b = 255 }
CYAN = { r = 0, g = 255, b = 255 }