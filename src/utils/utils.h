#ifndef UTILS_H
#define UTILS_H

#include <SDL2/SDL.h>

struct Colors {
    SDL_Color red = {255, 0, 0};
    SDL_Color green = {0, 255, 0};
    SDL_Color blue = {0, 0, 255};
    SDL_Color white = {255, 255, 255};
    SDL_Color black = {0, 0, 0};
    SDL_Color yellow = {255, 255, 0};
    SDL_Color magenta = {255, 0, 255};
    SDL_Color cyan = {0, 255, 255};
};

extern struct Colors col;

class Utils {
    public:

        static int Sign(int value);
        static int Clamp(int value, int min, int max);
        static float Clamp(float value, float min, float max);
        static float Distance(int x1, int y1, int x2, int y2);
        static float Lerp(float start, float end, float time);
        static float InvLerp(float a, float b, float v);
        static float Remap(float a, float b, float c, float d, float v);
        static int GetFPS();
};

#endif
