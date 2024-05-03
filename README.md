# dfhooks
Dwarf Fortress dfhooks API chainloader.

This library implements the dfhooks API as (defacto) defined in the DF
[g\_src](https://github.com/Putnam3145/Dwarf-Fortress--libgraphics--/blob/master/g_src/dfhooks.cpp)
sources and propagates the calls to other libraries that similarly implement
the API.

Those libraries must also be installed in the top-level Dwarf Fortress game
directory and conform to the following per-platform naming scheme:

- Windows: `dfhooks_{name}.dll` (e.g. `dfhooks_mylib.dll`)
- Linux: `libdfhooks_{name}.so`
- MacOS: `libdfhooks_{name}.dylib`

In addition to the symbols required by the dfhooks API, a library can also
export a `dfhooks_priority` symbol, which must be of type `int32_t`. Libraries
with lower values in the exported `dfhooks_priority` will be called first when
callbacks are propagated, except for shutdown, for which libraries with lower
values will be called last. If a library does not export a priority, the value
0 is used.

If you need to position yourself relative to DFHack, DFHack exports a priority
of 100.
