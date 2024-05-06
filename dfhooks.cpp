#include "ChainLoader.h"

#include <cstddef>
#include <iostream>

#ifdef WIN32
    #define DFhackCExport extern "C" __declspec(dllexport)
    #define DFhackDataExport extern "C" __declspec(dllexport)
#else
    #define DFhackCExport extern "C" __attribute__ ((visibility("default")))
    #define DFhackDataExport __attribute__ ((visibility("default")))
#endif

static ChainLoader * chain = NULL;

// called from the main thread before the simulation thread is started
// and the main event loop is initiated
DFhackCExport void dfhooks_init() {
    // don't initialize if already initialized
    if (chain)
        return;
    try {
        chain = new ChainLoader();
        chain->init();
    } catch (std::exception& e) {
        std::cerr << "Failed to load dfhooks libraries:" << e.what() << std::endl;
    }
}

// called from the main thread after the main event loops exits
DFhackCExport void dfhooks_shutdown() {
    if (!chain)
        return;
    chain->shutdown();
    delete chain;
    chain = NULL;
}

// called from the simulation thread in the main event loop
DFhackCExport void dfhooks_update() {
    if (!chain)
        return;
    chain->update();
}

// called from the simulation thread just before adding the macro
// recording/playback overlay
DFhackCExport void dfhooks_prerender() {
    if (!chain)
        return;
    chain->prerender();
}

// called from the main thread for each SDL event. if true is returned, then
// the event has been consumed and further processing shouldn't happen
DFhackCExport bool dfhooks_sdl_event(SDL_Event* event) {
    if (!chain)
        return false;
    return chain->sdl_event(event);
}

// called from the main thread for each utf-8 char read from the ncurses input
// key is positive for ncurses keys and negative for everything else
// if true is returned, then the event has been consumed and further processing
// shouldn't happen
DFhackCExport bool dfhooks_ncurses_key(int key) {
    if (!chain)
        return false;
    return chain->ncurses_key(key);
}
