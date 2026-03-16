#include "ChainLoader.h"

#include <filesystem>
#include <queue>
#include <fstream>
#include <string_view>

using std::string;
using std::vector;

using std::string_view_literals::operator""sv;

// look for libraries with these prefixes and extensions.
// .ini files are expected to contain a single line with the path to the library to load, which allows for loading libraries with non-standard names or from non-standard locations
// note that if both a .ini and a .dll/.so are present _both_ will be loaded, which may have unintended consequences if they point to the same library, so use with caution

constexpr auto search_prefix = {"dfhooks_"sv, "libdfhooks_"sv};
constexpr auto search_ext = {".dll"sv, ".so"sv};
constexpr auto search_ext_ini = ".ini"sv;

ChainLoader::ChainLoader() {
    auto cmp = [](const LibWrapper * lhs, const LibWrapper * rhs) { return lhs->priority < rhs->priority; };
    std::priority_queue<LibWrapper*, vector<LibWrapper*>, decltype(cmp)> priq(cmp);

    for (auto const& dir_entry : std::filesystem::directory_iterator{"."})
    {
        auto fname = dir_entry.path().filename();
        bool fnd = false;
        for (auto& prefix : search_prefix) {
            if (fname.stem().string().find(prefix) == 0) {
                fnd = true;
                break;
            }
        }
        if (!fnd) continue;

        if (fname.extension() == search_ext_ini)
        {
            std::string indirection;
            std::ifstream ini_file{fname};
            std::getline(ini_file, indirection);
            if (!indirection.empty())
            {
                std::filesystem::path lib{indirection};
                auto wrapper = new LibWrapper(lib);
                if (wrapper->handle)
                    priq.emplace(wrapper);
                else
                    delete wrapper;
            }
        }
        else for (auto& ext : search_ext)
        {
            if (fname.extension() == ext)
            {
                auto wrapper = new LibWrapper(fname);
                if (wrapper->handle)
                    priq.emplace(wrapper);
                else
                    delete wrapper;
            }
        }
    }
    while (!priq.empty()) {
        auto lib = priq.top();
        libs.emplace_back(lib);
        priq.pop();
    }
}

ChainLoader::~ChainLoader() {
    for (auto & lib : libs)
        delete lib;
    libs.clear();
}

void ChainLoader::init() {
    for (auto & lib : libs) {
        if (lib->init)
            lib->init();
    }
}

void ChainLoader::shutdown() {
    // shut down in reverse order
    for (auto libit = libs.rbegin(); libit != libs.rend(); ++libit) {
        if ((*libit)->shutdown)
            (*libit)->shutdown();
    }
}

void ChainLoader::update() {
    for (auto & lib : libs) {
        if (lib->update)
            lib->update();
    }
}

void ChainLoader::prerender() {
    for (auto & lib : libs) {
        if (lib->prerender)
            lib->prerender();
    }
}

bool ChainLoader::sdl_event(SDL_Event* event) {
    for (auto & lib : libs) {
        if (lib->sdl_event)
            if (lib->sdl_event(event))
                return true;
    }
    return false;
}

void ChainLoader::sdl_loop() {
    for (auto & lib : libs) {
        if (lib->sdl_loop)
            lib->sdl_loop();
    }
}

bool ChainLoader::ncurses_key(int key) {
    for (auto & lib : libs) {
        if (lib->ncurses_key)
            if (lib->ncurses_key(key))
                return true;
    }
    return false;
}
