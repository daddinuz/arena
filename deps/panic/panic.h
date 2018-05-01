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

#include <stdarg.h>

#if !(defined(__GNUC__) || defined(__clang__))
#define __attribute__(...)
#endif

#define PANIC_VERSION_MAJOR       0
#define PANIC_VERSION_MINOR       1
#define PANIC_VERSION_PATCH       0
#define PANIC_VERSION_SUFFIX      ""
#define PANIC_VERSION_IS_RELEASE  0
#define PANIC_VERSION_HEX         0x000100

/**
 * @return The semantic versioning string of the package.
 */
extern const char *
Panic_version(void)
__attribute__((__warn_unused_result__));

/**
 * Reports the error and aborts execution.
 * @warning this function should not be called directly, use the exported macro instead.
 */
extern void
__Panic_abort(const char *file, int line, const char *format, ...)
__attribute__((__nonnull__(1, 3), __format__(__printf__, 3, 4), __noreturn__));

/**
 * Like __panic but takes a variadic list.
 *
 * @see __panic(const char *file, int line, const char *format, ...)
 *
 * @warning this function should not be called directly, use the exported macro instead.
 */
extern void
__Panic_vabort(const char *file, int line, const char *format, va_list args)
__attribute__((__nonnull__(1, 3), __format__(__printf__, 3, 0), __noreturn__));

/**
 * This macro acts like a wrapper over __panic function passing the right
 * call coordinates (file and line number) for a better error report.
 */
#define Panic_abort(...) \
    __Panic_abort(__FILE__, __LINE__, __VA_ARGS__)

#ifdef __cplusplus
}
#endif
