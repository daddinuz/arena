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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#if !defined(ARENA_DEFAULT_CAPACITY)
#define ARENA_DEFAULT_CAPACITY  512u
#endif

#if !defined(ARENA_MIN_CAPACITY)
#define ARENA_MIN_CAPACITY      32u
#endif

#if !defined(__GNUC__)
#define __attribute__(...)
#endif

struct Arena;

/**
 * Creates a new arena with default capacity.
 * 
 * @attention Out of memory is a checked runtime error.
 */
extern struct Arena *Arena_default(void)
__attribute__((__warn_unused_result__));

/**
 * Creates a new arena with at least the specified capacity.
 * 
 * @attention (0 == capacity) is a checked runtime error.
 * @attention Out of memory is a checked runtime error.
 */
extern struct Arena *Arena_withCapacity(size_t capacity)
__attribute__((__warn_unused_result__, __alloc_size__(1)));

/**
 * Returns a block of allocated memory of the specified size using the specified alignment.
 *
 * @attention (NULL == self) is a checked runtime error.
 * @attention Invalid alignment values are checked runtime errors.
 * @attention (0 == size) is a checked runtime error.
 * @attention Out of memory is a checked runtime error.
 */
extern void *Arena_allocate(struct Arena *self, size_t alignment, size_t size)
__attribute__((__warn_unused_result__, __nonnull__(1), __alloc_size__(3)));

/**
 * Clones and returns the object pointed by data.
 *
 * @attention (NULL == self) is a checked runtime error.
 * @attention (NULL == data) is a checked runtime error.
 * @attention Invalid alignment values are checked runtime errors.
 * @attention (0 == size) is a checked runtime error.
 * @attention Out of memory is a checked runtime error.
 */
extern void *Arena_clone(struct Arena *self, const void *data, size_t alignment, size_t size)
__attribute__((__warn_unused_result__, __nonnull__(1, 2), __alloc_size__(4)));

/**
 * Clears the content of the arena (without releasing memory).
 * All references obtained by calling Arena_allocate before calling
 * this function are invalidated.
 * 
 * @attention (NULL == self) is a checked runtime error.
 */
extern void Arena_clear(struct Arena *self)
__attribute__((__nonnull__(1)));

/**
 * Drops the arena releasing memory.
 * All references obtained by calling Arena_allocate before calling
 * this function are invalidated.
 * 
 * After calling this method self is invalidated.
 * 
 * @attention (NULL == self) is a checked runtime error.
 */
extern void Arena_drop(struct Arena *self)
__attribute__((__nonnull__(1)));

/**
 * Gets the size of the biggest segment of memory currently available.
 * 
 * @attention (NULL == self) is a checked runtime error.
 */
extern size_t Arena_available(const struct Arena *self)
__attribute__((__warn_unused_result__, __nonnull__(1)));

/**
 * Gets the capacity of the arena.
 * 
 * @attention (NULL == self) is a checked runtime error.
 */
extern size_t Arena_capacity(const struct Arena *self)
__attribute__((__warn_unused_result__, __nonnull__(1)));

/**
 * Gets the size of the memory currently in use by the arena (including slop memory).
 * 
 * @attention (NULL == self) is a checked runtime error.
 */
extern size_t Arena_size(const struct Arena *self)
__attribute__((__warn_unused_result__, __nonnull__(1)));

#ifdef __cplusplus
}
#endif
