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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#if !(defined(__GNUC__) || defined(__clang__))
#define __attribute__(...)
#endif

#define ARENA_VERSION_MAJOR       0
#define ARENA_VERSION_MINOR       4
#define ARENA_VERSION_PATCH       0
#define ARENA_VERSION_SUFFIX      ""
#define ARENA_VERSION_IS_RELEASE  0
#define ARENA_VERSION_HEX         0x000400

/**
 * Arena is a region of memory which holds a collection of allocated objects
 * that can be efficiently de-allocated all at once.
 *
 * @attention Every function in this library terminates the program in case of out of memory.
 */
struct Arena;

/**
 * Creates a new arena with default capacity.
 *
 * @return A new arena instance.
 */
extern struct Arena *Arena_new(void)
__attribute__((__warn_unused_result__));

/**
 * Creates a new arena with at least the suggested capacity.
 *
 * @param capacityHint The suggested capacity for the arena (if 0 a default capacity will be used).
 * @return A new arena instance.
 */
extern struct Arena *Arena_withCapacity(size_t capacityHint)
__attribute__((__warn_unused_result__));

/**
 * Returns a block of allocated memory of the specified size using the strictest alignment.
 *
 * @param self The arena instance.
 * @param size The size of the requested memory block.
 *
 * @attention self must not be NULL.
 * @attention size must be greater than 0.
 *
 * @return The requested memory block.
 */
extern void *Arena_request(struct Arena *self, size_t size)
__attribute__((__warn_unused_result__, __nonnull__));

/**
 * Returns a block of allocated memory of the specified size.
 *
 * @param self The arena instance.
 * @param alignment The alignment for the memory block.
 * @param size The size of the requested memory block.
 *
 * @attention self must not be NULL.
 * @attention alignment must not be NULL.
 * @attention alignment must be a power of 2.
 * @attention size must be greater than 0.
 * @attention size must be an integral multiple of alignment.
 *
 * @return The requested memory block.
 */
extern void *Arena_requestWithAlignment(struct Arena *self, size_t alignment, size_t size)
__attribute__((__warn_unused_result__, __nonnull__));

/**
 * Gets the size of the memory currently in use by the arena.
 *
 * @param self The arena instance.
 * @attention self must not be NULL.
 * @return The size of the memory currently in use by the arena.
 */
extern size_t Arena_getSize(const struct Arena *self)
__attribute__((__warn_unused_result__, __nonnull__));

/**
 * Gets the size of the slop memory currently in use by the arena.
 *
 * @param self The arena instance.
 * @attention self must not be NULL.
 * @return The size of the slop memory currently in use by the arena.
 */
extern size_t Arena_getSlop(const struct Arena *self)
__attribute__((__warn_unused_result__, __nonnull__));

/**
 * Gets the current capacity of the arena.
 *
 * @param self The arena instance.
 * @attention self must not be NULL.
 * @return The current capacity of the arena.
 */
extern size_t Arena_getCapacity(const struct Arena *self)
__attribute__((__warn_unused_result__, __nonnull__));

/**
 * Gets the size of the biggest segment of memory currently available.
 *
 * @param self The arena instance.
 * @attention self must not be NULL.
 * @return The size of the biggest segment of memory currently available.
 */
extern size_t Arena_getBestAvailable(const struct Arena *self)
__attribute__((__warn_unused_result__, __nonnull__));

/**
 * Gets the size of the smallest segment of memory currently available.
 *
 * @param self The arena instance.
 * @attention self must not be NULL.
 * @return The size of the smallest segment of memory currently available.
 */
extern size_t Arena_getWorstAvailable(const struct Arena *self)
__attribute__((__warn_unused_result__, __nonnull__));

/**
 * Gets the number of chunks held by the arena.
 *
 * @param self The arena instance.
 * @attention self must not be NULL.
 * @return The number of chunks held by the arena.
 */
extern size_t Arena_getChunks(const struct Arena *self)
__attribute__((__warn_unused_result__, __nonnull__));

/**
 * Frees the memory held by the arena without deallocating it.
 * Any reference to memory obtained before calling this function is invalidated.
 *
 * @param self The arena instance.
 * @attention self must not be NULL.
 */
extern void Arena_clear(struct Arena *self)
__attribute__((__nonnull__));

/**
 * Shrinks the arena deallocating chunks not in use.
 *
 * @param self The arena instance.
 * @attention self must not be NULL.
 */
extern void Arena_shrink(struct Arena *self)
__attribute__((__nonnull__));

/**
 * Deletes an arena.
 *
 * @param self The arena instance.
 */
extern void Arena_delete(struct Arena *self);

#ifdef __cplusplus
}
#endif
