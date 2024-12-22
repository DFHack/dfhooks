#include "ChainLoader.h"

#include <filesystem>
#include <queue>

using std::string;
using std::vector;

#if _WIN32
static const string search_prefix = "dfhooks_";
static const string search_ext = ".dll";
#else
static const string search_prefix = "libdfhooks_";
#  if __APPLE__
static const string search_ext = ".dylib";
#  else
static const string search_ext = ".so";
#  endif
#endif

ChainLoader::ChainLoader() {
    auto cmp = [](const LibWrapper * lhs, const LibWrapper * rhs) { return lhs->priority < rhs->priority; };
    std::priority_queue<LibWrapper*, vector<LibWrapper*>, decltype(cmp)> priq(cmp);

    for (auto const& dir_entry : std::filesystem::directory_iterator{"."}) {
        auto fname = dir_entry.path().filename();
        if (fname.extension() != search_ext || fname.stem().string().find(search_prefix) != 0)
            continue;
        auto wrapper = new LibWrapper(fname.string());
        if (wrapper->handle)
            priq.emplace(wrapper);
        else
            delete wrapper;
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
