#include "LibWrapper.h"

using std::string;

#if _WIN32
#  include <windows.h>
#else
#  include <dlfcn.h>
#endif

static void* open_library(const string& fname) {
#if _WIN32
    return LoadLibrary(fname.c_str());
#else
    return dlopen(fname.c_str(), RTLD_LAZY);
#endif
}

static void close_library(void* handle) {
    if (!handle) {
        return;
    }
#if _WIN32
    FreeLibrary((HMODULE)handle);
#else
    dlclose(handle);
#endif
}

static void* load_sym(void* handle, const char* sym) {
#if _WIN32
    return (void*)GetProcAddress((HMODULE)handle, sym);
#else
    return dlsym(handle, sym);
#endif
}

LibWrapper::LibWrapper(const string& fname) {
    handle = open_library(fname);
    if (!handle)
        return;

    int32_t * priority_sym = (int *)load_sym(handle, "dfhooks_priority");
    if (priority_sym) priority = *priority_sym;

    init = (dfhooks_init_fn)load_sym(handle, "dfhooks_init");
    shutdown = (dfhooks_shutdown_fn)load_sym(handle, "dfhooks_shutdown");
    update = (dfhooks_update_fn)load_sym(handle, "dfhooks_update");
    prerender = (dfhooks_prerender_fn)load_sym(handle, "dfhooks_prerender");
    sdl_event = (dfhooks_sdl_event_fn)load_sym(handle, "dfhooks_sdl_event");
    sdl_loop = (dfhooks_sdl_loop_fn)load_sym(handle, "dfhooks_sdl_loop");
    ncurses_key = (dfhooks_ncurses_key_fn)load_sym(handle, "dfhooks_ncurses_key");
}

LibWrapper::~LibWrapper() {
    close_library(handle);
    handle = nullptr;
}
