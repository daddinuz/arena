/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Davide Di Carlo
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include <panic/panic.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include <assert.h>
#include "arena.h"

// Taken from Bit Twiddling Hacks: http://graphics.stanford.edu/~seander/bithacks.html#DetermineIfPowerOf2
#define __isPowerOfTwo(n)   (n && !(n & (n - 1u)))

static_assert(ARENA_DEFAULT_CAPACITY >= ARENA_MIN_CAPACITY, "ARENA_DEFAULT_CAPACITY must be >= ARENA_MIN_CAPACITY");
static_assert(__isPowerOfTwo(ARENA_DEFAULT_CAPACITY), "ARENA_DEFAULT_CAPACITY must be a power of 2");
static_assert(__isPowerOfTwo(ARENA_MIN_CAPACITY), "ARENA_MIN_CAPACITY must be a power of 2");
static_assert(sizeof(char) == 1u, "Unexpected char size");

#define maxAlign    alignof(max_align_t)

static inline __attribute__((__warn_unused_result__))
size_t max(const size_t a, const size_t b) {
    return a > b ? a : b;
}

static inline __attribute__((__warn_unused_result__))
bool isPowerOf2(const size_t n) {
    return __isPowerOfTwo(n);
}

static inline __attribute__((__warn_unused_result__, __nonnull__(1)))
bool isAligned(const void *const memory, const size_t alignment) {
    assert(NULL != memory);
    assert(alignment <= maxAlign);
    assert(isPowerOf2(alignment));
    return 0u == (((uintptr_t) memory) & (((uintptr_t) alignment) - 1u));
}

static inline __attribute__((__warn_unused_result__))
void *align(void *memory, const size_t alignment) {
    assert(alignment <= maxAlign);
    assert(isPowerOf2(alignment));
    memory = (void *) ((((uintptr_t) memory) + alignment - 1u) & ~(((uintptr_t) alignment) - 1u));
    assert(isAligned(memory, alignment));
    return memory;
}

struct Arena {
    alignas(max_align_t)
    size_t capacity;
    size_t offset;
    char memory[];
};

struct Arena *Arena_default(void) {
    return Arena_withCapacity(ARENA_DEFAULT_CAPACITY);
}

struct Arena *Arena_withCapacity(const size_t capacity) {
    assert(capacity > 0u);
    const size_t actualCapacity = max(ARENA_MIN_CAPACITY, capacity);
    struct Arena *const self = calloc(1u, sizeof(*self) + actualCapacity);

    if (NULL != self) {
        self->capacity = actualCapacity;
        return self;
    }

    panic("Out of memory");
}

void *Arena_allocate(struct Arena *const self, const size_t alignment, const size_t size) {
    assert(NULL != self);
    assert(alignment <= maxAlign);
    assert(isPowerOf2(alignment));
    assert(size > 0u);
    char *const address = &self->memory[self->offset];
    char *const alignedAddress = align(address, alignment);
    const size_t padding = alignedAddress - address;

    if ((self->offset + padding + size) > self->capacity) {
        panic("Out of memory");
    }

    self->offset += padding + size;
    return alignedAddress;
}

void *Arena_clone(struct Arena *const self, const void *const data, const size_t alignment, const size_t size) {
    assert(NULL != self);
    assert(NULL != data);
    assert(alignment <= maxAlign);
    assert(isPowerOf2(alignment));
    assert(size > 0u);
    return memcpy(Arena_allocate(self, alignment, size), data, size);
}

void Arena_clear(struct Arena *const self) {
    assert(NULL != self);
    const size_t capacity = self->capacity;
    memset(self, 0u, sizeof(*self) + self->offset);
    self->capacity = capacity;
}

void Arena_drop(struct Arena *const self) {
    assert(NULL != self);
    free(self);
}

size_t Arena_available(const struct Arena *const self) {
    assert(NULL != self);
    return self->capacity - self->offset;
}

size_t Arena_capacity(const struct Arena *const self) {
    assert(NULL != self);
    return self->capacity;
}

size_t Arena_size(const struct Arena *const self) {
    assert(NULL != self);
    return self->offset;
}
