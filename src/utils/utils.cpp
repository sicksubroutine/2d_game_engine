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