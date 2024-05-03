#pragma once

#include <string>

union SDL_Event;

typedef void (*dfhooks_init_fn)();
typedef void (*dfhooks_shutdown_fn)();
typedef void (*dfhooks_update_fn)();
typedef void (*dfhooks_prerender_fn)();
typedef bool (*dfhooks_sdl_event_fn)(SDL_Event* event);
typedef bool (*dfhooks_ncurses_key_fn)(int key);

struct LibWrapper {
    void* handle = nullptr;
    int priority = 0;
    dfhooks_init_fn init = nullptr;
    dfhooks_shutdown_fn shutdown = nullptr;
    dfhooks_update_fn update = nullptr;
    dfhooks_prerender_fn prerender = nullptr;
    dfhooks_sdl_event_fn sdl_event = nullptr;
    dfhooks_ncurses_key_fn ncurses_key = nullptr;

    LibWrapper(const std::string& fname);
    virtual ~LibWrapper();
};
