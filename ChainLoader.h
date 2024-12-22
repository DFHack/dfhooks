#pragma once

#include "LibWrapper.h"

#include <vector>

class ChainLoader {
public:
    ChainLoader();
    ~ChainLoader();

    void init();
    void shutdown();
    void update();
    void prerender();
    bool sdl_event(SDL_Event* event);
    void sdl_loop();
    bool ncurses_key(int key);

private:
    std::vector<LibWrapper *> libs;
};
