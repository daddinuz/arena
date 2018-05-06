/*
 * Author: daddinuz
 * email:  daddinuz@gmail.com
 *
 * Copyright (c) 2018 Davide Di Carlo
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

#include <assert.h>
#include <memory.h>
#include <stdint.h>
#include <stdalign.h>
#include <alligator/alligator.h>
#include "arena.h"
#include "arena_config.h"

#if (defined(__STDC_VERSION__) && __STDC_VERSION__ < 201112L) || (defined(__cplusplus) && __cplusplus < 201103L)
    #error "C/C++ 11 support is required"
#endif

#if ARENA_DEFAULT_CHUNK_CAPACITY <= 0
    #error "ARENA_DEFAULT_CHUNK_CAPACITY must be greater than zero."
#endif

#define _STR(x)     #x
#define STR(x)      _STR(x)

#define assertIsPowerOfTwo(xNumber) \
    do { size_t __xNumber = (xNumber); assert((__xNumber != 0) && ((__xNumber & (~__xNumber + 1)) == __xNumber)); } while (false)

#define assertIsIntegralMultiple(xAlignment, xSize) \
    do { size_t __xAlignment = (xAlignment), __xSize = (xSize); assert((__xSize % __xAlignment) == 0); } while (false)

const char *Arena_version(void) {
    return STR(ARENA_VERSION_MAJOR) "." STR(ARENA_VERSION_MINOR) "." STR(ARENA_VERSION_PATCH) ARENA_VERSION_SUFFIX;
}

static const size_t maxAlign = alignof(max_align_t);

static inline __attribute__((__warn_unused_result__)) size_t max(const size_t a, const size_t b) {
    return a > b ? a : b;
}

static inline __attribute__((__warn_unused_result__)) size_t min(const size_t a, const size_t b) {
    return a < b ? a : b;
}

static inline __attribute__((__warn_unused_result__)) size_t roundToAlignBoundary(size_t alignment, size_t size) {
    assert(alignment > 0);
    assert(size > 0);
    return ((size + alignment - 1) / alignment) * alignment;
}

struct Chunk {
    size_t size;
    size_t capacity;
    struct Chunk *next;
    char memory[];
};

static __attribute__((__warn_unused_result__)) OptionOf(struct Chunk *) Chunk_new(size_t capacityHint) {
    struct Chunk *self;
    const size_t capacity = roundToAlignBoundary(maxAlign, max(capacityHint, ARENA_DEFAULT_CHUNK_CAPACITY));
    Option option = Alligator_aligned_alloc(maxAlign, roundToAlignBoundary(maxAlign, offsetof(struct Chunk, memory)) +
                                                      capacity * sizeof(self->memory[0]));
    if (Option_isSome(option)) {
        self = Option_unwrap(option);
        self->size = 0;
        self->capacity = capacity;
        self->next = NULL;
        return Option_some(self);
    }
    return None;
}

static inline void Chunk_delete(struct Chunk *self) {
    Alligator_free(self);
}

struct Arena {
    struct Chunk *head;
    size_t capacity;
    size_t chunks;
    size_t size;
    size_t slop;
};

OptionOf(struct Arena *) Arena_new(void) {
    return Arena_withCapacity(ARENA_DEFAULT_CHUNK_CAPACITY);
}

OptionOf(struct Arena *) Arena_withCapacity(const size_t capacityHint) {
    Option option;
    struct Arena *self;

    option = Alligator_aligned_alloc(alignof(*self), sizeof(*self));
    if (Option_isSome(option)) {
        self = Option_unwrap(option);
        option = Chunk_new(capacityHint);
        if (Option_isSome(option)) {
            struct Chunk *chunk = Option_unwrap(option);
            self->size = self->slop = 0;
            self->capacity = chunk->capacity;
            self->chunks = 1;
            self->head = chunk;
            return Option_some(self);
        }
        Alligator_free(self);
    }

    return None;
}

OptionOf(void *) Arena_request(struct Arena *const self, const size_t size) {
    assert(self);
    assert(self->head);
    assert(size > 0);
    return Arena_requestWithAlignment(self, maxAlign, roundToAlignBoundary(maxAlign, size));
}

OptionOf(void *) Arena_requestWithAlignment(struct Arena *const self, const size_t alignment, const size_t size) {
    assert(self);
    assert(self->head);
    assert(alignment > 0);
    assert(size > 0);
    assertIsPowerOfTwo(alignment);
    assertIsIntegralMultiple(alignment, size);

    size_t slop;
    void *memory;
    for (struct Chunk *chunk = self->head; chunk; chunk = chunk->next) {
        memory = chunk->memory + chunk->size;
        slop = (((((uintptr_t) memory) + alignment - 1) / alignment) * alignment) - (uintptr_t) memory;
        if (chunk->size + slop + size <= chunk->capacity) {
            memory += slop;
            chunk->size += slop + size;
            self->size += size;
            self->slop += slop;
            assertIsIntegralMultiple(alignment, (uintptr_t) memory);
            return Option_some(memory);
        }
    }

    Option option = Chunk_new(size);
    if (Option_isSome(option)) {
        struct Chunk *chunk = Option_unwrap(option);
        assertIsIntegralMultiple(alignment, (uintptr_t) chunk->memory);
        chunk->size += size;
        chunk->next = self->head;
        self->head = chunk;
        self->chunks += 1;
        self->size += size;
        self->capacity += chunk->capacity;
        return Option_some(chunk->memory);
    }

    return None;
}

size_t Arena_getSize(const struct Arena *const self) {
    assert(self);
    assert(self->head);
    return self->size;
}

size_t Arena_getSlop(const struct Arena *const self) {
    assert(self);
    assert(self->head);
    return self->slop;
}

size_t Arena_getCapacity(const struct Arena *const self) {
    assert(self);
    assert(self->head);
    return self->capacity;
}

size_t Arena_getBestAvailable(const struct Arena *const self) {
    assert(self);
    assert(self->head);
    size_t bestAvailable = 0;
    for (const struct Chunk *chunk = self->head; chunk; chunk = chunk->next) {
        bestAvailable = max(bestAvailable, chunk->capacity - chunk->size);
    }
    return bestAvailable;
}

size_t Arena_getWorstAvailable(const struct Arena *const self) {
    assert(self);
    assert(self->head);
    size_t worstAvailable = SIZE_MAX;
    for (const struct Chunk *chunk = self->head; chunk; chunk = chunk->next) {
        worstAvailable = min(worstAvailable, chunk->capacity - chunk->size);
    }
    return worstAvailable;
}

size_t Arena_getChunks(const struct Arena *const self) {
    assert(self);
    assert(self->head);
    return self->chunks;
}

void Arena_clear(struct Arena *const self) {
    assert(self);
    assert(self->head);
    for (struct Chunk *chunk = self->head; chunk; chunk = chunk->next) {
        chunk->size = 0;
        memset(chunk->memory, 0, chunk->capacity);
    }
    self->size = self->slop = 0;
}

void Arena_shrink(struct Arena *const self) {
    assert(self);
    assert(self->head);
    struct Chunk *const head = self->head;
    for (struct Chunk *chunk = head->next; chunk; chunk = chunk->next) {
        if (chunk->size == 0) {
            head->next = chunk->next;
            self->chunks -= 1;
            self->capacity -= chunk->capacity;
            Chunk_delete(chunk);
            chunk = head;
        }
    }
}

void Arena_delete(struct Arena *self) {
    if (self) {
        assert(self->head);
        for (struct Chunk *chunk = self->head; chunk; chunk = self->head) {
            self->head = chunk->next;
            Chunk_delete(chunk);
        }
        Alligator_free(self);
    }
}
