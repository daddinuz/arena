/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Davide Di Carlo
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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "panic.h"

#define NEWLINE "\r\n"

static PanicHandler globalPanicHandler = NULL;

static void terminate(const char *trace, const char *format, va_list args)
__attribute__((__noreturn__, __nonnull__(1, 2), __format__(__printf__, 2, 0)));

static void backtrace(FILE *stream)
__attribute__((__nonnull__));

PanicHandler panic_register(const PanicHandler handler) {
    const PanicHandler backup = globalPanicHandler;
    globalPanicHandler = handler;
    return backup;
}

void __panic(const char *const trace, const char *const format, ...) {
    assert(NULL != trace);
    assert(NULL != format);
    va_list args;
    va_start(args, format);
    terminate(trace, format, args);
}

void __vpanic(const char *const trace, const char *const format, va_list args) {
    assert(NULL != trace);
    assert(NULL != format);
    terminate(trace, format, args);
}

void __panic_when(const bool condition, const char *const trace, const char *const format, ...) {
    assert(NULL != trace);
    assert(NULL != format);
    if (condition) {
        va_list args;
        va_start(args, format);
        terminate(trace, format, args);
    }
}

void __panic_unless(const bool condition, const char *const trace, const char *const format, ...) {
    assert(NULL != trace);
    assert(NULL != format);
    if (!condition) {
        va_list args;
        va_start(args, format);
        terminate(trace, format, args);
    }
}

void terminate(const char *const trace, const char *const format, va_list args) {
    assert(NULL != trace);
    assert(NULL != format);
    fputs(NEWLINE, stderr);
    backtrace(stderr);
    fprintf(stderr, "   At: '%s'" NEWLINE, trace);

    if (0 != errno) {
        fprintf(stderr, "Error: (%d) %s" NEWLINE, errno, strerror(errno));
    }

    fputs("Cause: ", stderr);
    vfprintf(stderr, format, args);
    fputs(NEWLINE, stderr);
    va_end(args);

    if (NULL != globalPanicHandler) {
        globalPanicHandler();
    }

    abort();
}

#if PANIC_UNWIND_SUPPORT

#define N_SIZE 8
#define M_SIZE 32

#define UNW_LOCAL_ONLY

#include <libunwind.h>

void backtrace(FILE *const stream) {
    assert(NULL != stream);
    char buffer[N_SIZE][M_SIZE + 1] = {{0}};
    unw_context_t context = {0};
    unw_cursor_t cursor = {0};
    const int previousError = errno;
    size_t size = 0;

    unw_getcontext(&context);
    unw_init_local(&cursor, &context);

    // skip: terminate and panics function calls
    for (size_t i = 0; i < 2; ++i) {
        if (unw_step(&cursor) <= 0) {
            errno = previousError;  // restore errno
            return;                 // something wrong, exit
        }
    }

    while (size < N_SIZE &&
           unw_step(&cursor) > 0 &&
           unw_get_proc_name(&cursor, buffer[size], M_SIZE, NULL) == 0 &&
           strcmp("main", buffer[size++]) != 0);

    if (0 < size) {
        fputs("Traceback (most recent call last):" NEWLINE, stream);

        if (0 != strcmp("main", buffer[size - 1])) {
            fputs("  [ ]: (...)" NEWLINE, stream);
        }

        for (size_t i = 1; i < size; ++i) {
            fprintf(stream, "  [%zu]: (%s)" NEWLINE, i - 1, buffer[size - i]);
        }

        fprintf(stream, "  ->-: (%s) current function" NEWLINE, buffer[0]);
        fputs(NEWLINE, stream);
    }

    errno = previousError;  // restore errno
}

#else

void backtrace(FILE *const stream) {
    assert(NULL != stream);
    (void) stream;
}

#endif
