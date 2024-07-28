#ifndef MTCUTE_WASM_H
#define MTCUTE_WASM_H

#include <emscripten.h>

#include "common_defs.h"

#define WASM_EXPORT __attribute__((used))

#include <stdlib.h>

#define memset(p, v, n) __builtin_memset(p, v, n)
#define memcpy(d, s, n) __builtin_memcpy(d, s, n)

// more than enough for most of our cases
extern uint8_t shared_out[256];

#endif  // MTCUTE_WASM_H