#include "utils.h"
#include <cmath>

struct Colors col;

float Utils::Distance(int x1, int y1, int x2, int y2) {
    int dx = x1 - x2;
    int dy = y1 - y2;
    return sqrt(pow(dx, 2) + pow(dy, 2));
}

int Utils::Sign(int value) {
    return (value > 0) - (value < 0);
}

float Utils::Clamp(float value, float min, float max) {
    if (value < min) {
        return min;
    } else if (value > max) {
        return max;
    } else {
        return value;
    }
}

int Utils::Clamp(int value, int min, int max) {
    if (value < min) {
        return min;
    } else if (value > max) {
        return max;
    } else {
        return value;
    }
}

float Utils::Lerp(float a, float b, float f) {
    return a + f * (b - a);
}

float Utils::InvLerp(float a, float b, float v) {
    return (v - a) / (b - a);
}

float Utils::Remap(float a, float b, float c, float d, float v) {
    float t = InvLerp(a, b, v);
    return Lerp(c, d, t);
}

int Utils::GetFPS() {
    static int frame_count = 0;
    static int fps = 0;
    static int last_time = 0;
    int current_time = SDL_GetTicks();
    frame_count++;
    if (current_time > last_time + 1000) {
        last_time = current_time;
        fps = frame_count;
        frame_count = 0;
    }
    return fps;
}

bool Utils::IsWithinCircle(float x, float y, float x1, float y2, int radius) {
    float dx = x - x1;
    float dy = y - y2;
    return (dx * dx + dy * dy) <= (radius * radius);
}