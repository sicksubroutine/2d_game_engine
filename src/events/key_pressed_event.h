#ifndef KEY_PRESSED_EVENT_H
#define KEY_PRESSED_EVENT_H

#include "../ecs/ecs.h"
#include "../event_bus/event.h"
#include <SDL2/SDL.h>

class KeyPressedEvent: public Event {
    public:
        SDL_Keycode symbol;
        KeyPressedEvent(SDL_Keycode symbol): symbol(symbol) {}
};

#endif
