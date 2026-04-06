#pragma once

#include <string>
#include <filesystem>

union SDL_Event;

using dfhooks_preinit_fn = auto (std::filesystem::path) -> void;
using dfhooks_init_fn = auto () -> void;
using dfhooks_shutdown_fn = auto () -> void;
using dfhooks_update_fn = auto () -> void;
using dfhooks_prerender_fn = auto () -> void;
using dfhooks_sdl_event_fn = auto (SDL_Event* event) -> bool;
using dfhooks_sdl_loop_fn = auto () -> void;
using dfhooks_ncurses_key_fn = auto (int key) -> bool;

struct LibWrapper {
    void* handle = nullptr;
    int32_t priority = 0;
    dfhooks_preinit_fn* preinit = nullptr;
    dfhooks_init_fn* init = nullptr;
    dfhooks_shutdown_fn* shutdown = nullptr;
    dfhooks_update_fn* update = nullptr;
    dfhooks_prerender_fn* prerender = nullptr;
    dfhooks_sdl_event_fn* sdl_event = nullptr;
    dfhooks_sdl_loop_fn* sdl_loop = nullptr;
    dfhooks_ncurses_key_fn* ncurses_key = nullptr;

    LibWrapper(const std::filesystem::path& fname);
    virtual ~LibWrapper();
};
