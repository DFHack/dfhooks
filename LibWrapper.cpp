#include "LibWrapper.h"

#include <filesystem>
#include <iostream>

using std::string;
using std::filesystem::path;

#if _WIN32
#  include <windows.h>
#  include <libloaderapi.h>
#else
#  include <dlfcn.h>
#endif

static void* open_library(std::filesystem::path fname) {
#if _WIN32
    std::filesystem::path path = std::filesystem::canonical(fname);
    std::filesystem::path ppath = path.parent_path();
    [[maybe_unused]] DLL_DIRECTORY_COOKIE cookie = AddDllDirectory(ppath.wstring().c_str());
    auto handle = LoadLibraryExW(path.filename().wstring().c_str(), NULL, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
    return handle;
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

extern "C" void dfhooks_init();

LibWrapper::LibWrapper(const std::filesystem::path& fname) {
    handle = open_library(fname);
    if (!handle)
        return;

    int32_t * priority_sym = (int *)load_sym(handle, "dfhooks_priority");
    if (priority_sym) priority = *priority_sym;

    preinit = (dfhooks_preinit_fn)load_sym(handle, "dfhooks_preinit");
    init = (dfhooks_init_fn)load_sym(handle, "dfhooks_init");
    shutdown = (dfhooks_shutdown_fn)load_sym(handle, "dfhooks_shutdown");
    update = (dfhooks_update_fn)load_sym(handle, "dfhooks_update");
    prerender = (dfhooks_prerender_fn)load_sym(handle, "dfhooks_prerender");
    sdl_event = (dfhooks_sdl_event_fn)load_sym(handle, "dfhooks_sdl_event");
    sdl_loop = (dfhooks_sdl_loop_fn)load_sym(handle, "dfhooks_sdl_loop");
    ncurses_key = (dfhooks_ncurses_key_fn)load_sym(handle, "dfhooks_ncurses_key");

    if (preinit)
        preinit(fname);

    if (init == &dfhooks_init)
    {
        // if the library exports the same init function as the main dfhooks, initing the library will can an infinite recursion. disallow this.
        close_library(handle);
        handle = nullptr;
    }
}

LibWrapper::~LibWrapper() {
    close_library(handle);
    handle = nullptr;
}
