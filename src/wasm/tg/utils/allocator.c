#include "wasm.h"

// extern unsigned char __heap_base;
// static size_t __heap_tail = (size_t) &__heap_base;
// static size_t __heap_mark = (size_t) &__heap_base;

// #define memory_size() __builtin_wasm_memory_size(0)

// #define memory_grow(delta) __builtin_wasm_memory_grow(0, delta)

// enum {
// 	_mem_flag_used = 0xbf82583a,
// 	_mem_flag_free = 0xab34d705
// };


uint8_t shared_out[256];

WASM_EXPORT uint8_t* __get_shared_out() {
    return shared_out;
}