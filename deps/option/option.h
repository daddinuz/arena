/*
Author: daddinuz
email:  daddinuz@gmail.com

Copyright (c) 2018 Davide Di Carlo

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <stdbool.h>

#if !(defined(__GNUC__) || defined(__clang__))
__attribute__(...)
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define OPTION_VERSION_MAJOR        0
#define OPTION_VERSION_MINOR        25
#define OPTION_VERSION_PATCH        0
#define OPTION_VERSION_SUFFIX       ""
#define OPTION_VERSION_IS_RELEASE   0
#define OPTION_VERSION_HEX          0x002500

/**
 * An option type or option type is a polymorphic type that represents encapsulation of an optional value;
 * e.g. it is used as the return type of functions which may or may not return a meaningful value when they are applied.
 */

/**
 * Option variants
 */
enum OptionVariant {
    OptionVariant_None, OptionVariant_Some
};

/**
 * The type of the value wrapped by an `Option`
 */
typedef const void *OptionView_Value;

/**
 * @attention this struct must be treated as opaque therefore its members must not be accessed directly.
 */
typedef struct {
    OptionView_Value __value;
    enum OptionVariant __variant;
} OptionView;

/**
 * An helper macro used for type hinting, useful when writing interfaces.
 * By convention the annotated type is the wrapped value type.
 */
#define OptionViewOf(type) \
    OptionView

/**
 * The `NoneView` instance used to represent the absence of a value.
 */
extern const OptionView NoneView;

/**
 * Creates an `OptionView` wrapping a value.
 */
extern OptionView OptionView_some(OptionView_Value value)
__attribute__((__warn_unused_result__));

/**
 * Creates an `OptionView` wrapping a value.
 * If value is `NULL` returns an `OptionVariant_None` else returns an `OptionVariant_Some`.
 */
extern OptionView OptionView_fromNullable(OptionView_Value value)
__attribute__((__warn_unused_result__));

/**
 * Returns `true` if this `OptionView` is an `OptionVariant_None`, `false` otherwise.
 */
extern bool OptionView_isNone(OptionView self)
__attribute__((__warn_unused_result__));

/**
 * Returns `true` if this `OptionView` is an `OptionVariant_Some`, `false` otherwise.
 */
extern bool OptionView_isSome(OptionView self)
__attribute__((__warn_unused_result__));

/**
 * If this `OptionView` is an `OptionVariant_Some`, apply `f` on this value else return an `OptionVariant_None`.
 *
 * @attention f must not be `NULL`.
 */
extern OptionView OptionView_map(OptionView self, OptionView f(OptionView_Value))
__attribute__((__warn_unused_result__, __nonnull__(2)));

/**
 * If this `OptionView` is an `OptionVariant_Some` then it will be returned, if it is an `OptionVariant_None` the next `OptionView will be returned`.
 */
extern OptionView OptionView_alt(OptionView self, OptionView a)
__attribute__((__warn_unused_result__));

/**
 * Chains several possibly failing computations.
 *
 * @attention f must not be `NULL`.
 */
extern OptionView OptionView_chain(OptionView self, OptionView f(OptionView_Value))
__attribute__((__warn_unused_result__, __nonnull__(2)));

/**
 * Applies a function to each case in this `OptionView`.
 *
 * @attention whenNone must not be `NULL`.
 * @attention whenSome must not be `NULL`.
 */
extern OptionView_Value
OptionView_fold(OptionView self, OptionView_Value whenNone(void), OptionView_Value whenSome(OptionView_Value))
__attribute__((__warn_unused_result__, __nonnull__(2, 3)));

/**
 * Returns the value from this `OptionView` if it's an `OptionVariant_Some` or a default value if this is an `OptionVariant_None`.
 */
extern OptionView_Value OptionView_getOr(OptionView self, OptionView_Value defaultValue)
__attribute__((__warn_unused_result__));

/**
 * Returns the value from this `OptionView` if it's an `OptionVariant_Some` or compute a value if this is an `OptionVariant_None`.
 *
 * @attention f must not be `NULL`.
 */
extern OptionView_Value OptionView_getOrElse(OptionView self, OptionView_Value f(void))
__attribute__((__warn_unused_result__, __nonnull__(2)));

/**
 * Unwraps the value of this `OptionView` if it's an `OptionVariant_Some` or panics if this is an `OptionVariant_None`.
 */
#define OptionView_unwrap(self) \
    __OptionView_unwrap((__FILE__), (__LINE__), (self))

/**
 * Unwraps the value of this `OptionView` if it's an `OptionVariant_Some` or panics if this is an `OptionVariant_None` and prints a custom message.
 */
#define OptionView_expect(self, ...) \
    __OptionView_expect((__FILE__), (__LINE__), (self), __VA_ARGS__)

/**
 * @attention this function must be treated as opaque therefore must not be called directly.
 */
extern OptionView_Value __OptionView_unwrap(const char *file, int line, OptionView self)
__attribute__((__nonnull__(1)));

/**
 * @attention this function must be treated as opaque therefore must not be called directly.
 */
extern OptionView_Value __OptionView_expect(const char *file, int line, OptionView self, const char *format, ...)
__attribute__((__nonnull__(1, 4), __format__(__printf__, 4, 5)));

/*
 *
 */

/**
 * The type of the value wrapped by an `Option`
 */
typedef void *Option_Value;

/**
 * @attention this struct must be treated as opaque therefore its members must not be accessed directly.
 */
typedef struct {
    Option_Value __value;
    enum OptionVariant __variant;
} Option;

/**
 * An helper macro used for type hinting, useful when writing interfaces.
 * By convention the annotated type is the wrapped value type.
 */
#define OptionOf(type) \
    Option

/**
 * The `None` instance used to represent the absence of a value.
 */
extern const Option None;

/**
 * Creates an `Option` wrapping a value.
 */
extern Option Option_some(Option_Value value)
__attribute__((__warn_unused_result__));

/**
 * Creates an `Option` wrapping a value.
 * If value is `NULL` returns an `OptionVariant_None` else returns an `OptionVariant_Some`.
 */
extern Option Option_fromNullable(Option_Value value)
__attribute__((__warn_unused_result__));

/**
 * Creates an `OptionView` from this `Option`.
 */
extern OptionView Option_toView(Option self)
__attribute__((__warn_unused_result__));

/**
 * Returns `true` if this `Option` is an `OptionVariant_None`, `false` otherwise.
 */
extern bool Option_isNone(Option self)
__attribute__((__warn_unused_result__));

/**
 * Returns `true` if this `Option` is an `OptionVariant_Some`, `false` otherwise.
 */
extern bool Option_isSome(Option self)
__attribute__((__warn_unused_result__));

/**
 * If this `Option` is an `OptionVariant_Some`, apply `f` on this value else return an `OptionVariant_None`.
 *
 * @attention f must not be `NULL`.
 */
extern Option Option_map(Option self, Option f(Option_Value))
__attribute__((__warn_unused_result__, __nonnull__(2)));

/**
 * If this `Option` is an `OptionVariant_Some` then it will be returned, if it is an `OptionVariant_None` the next `Option will be returned`.
 */
extern Option Option_alt(Option self, Option a)
__attribute__((__warn_unused_result__));

/**
 * Chains several possibly failing computations.
 *
 * @attention f must not be `NULL`.
 */
extern Option Option_chain(Option self, Option f(Option_Value))
__attribute__((__warn_unused_result__, __nonnull__(2)));

/**
 * Applies a function to each case in this `Option`.
 *
 * @attention whenNone must not be `NULL`.
 * @attention whenSome must not be `NULL`.
 */
extern Option_Value Option_fold(Option self, Option_Value whenNone(void), Option_Value whenSome(Option_Value))
__attribute__((__warn_unused_result__, __nonnull__(2, 3)));

/**
 * Returns the value from this `Option` if it's an `OptionVariant_Some` or a default value if this is an `OptionVariant_None`.
 */
extern Option_Value Option_getOr(Option self, Option_Value defaultValue)
__attribute__((__warn_unused_result__));

/**
 * Returns the value from this `Option` if it's an `OptionVariant_Some` or compute a value if this is an `OptionVariant_None`.
 *
 * @attention f must not be `NULL`.
 */
extern Option_Value Option_getOrElse(Option self, Option_Value f(void))
__attribute__((__warn_unused_result__, __nonnull__(2)));

/**
 * Unwraps the value of this `Option` if it's an `OptionVariant_Some` or panics if this is an `OptionVariant_None`.
 */
#define Option_unwrap(self) \
    __Option_unwrap((__FILE__), (__LINE__), (self))

/**
 * Unwraps the value of this `Option` if it's an `OptionVariant_Some` or panics if this is an `OptionVariant_None` and prints a custom message.
 */
#define Option_expect(self, ...) \
    __Option_expect((__FILE__), (__LINE__), (self), __VA_ARGS__)

/**
 * @attention this function must be treated as opaque therefore must not be called directly.
 */
extern Option_Value __Option_unwrap(const char *file, int line, Option self)
__attribute__((__nonnull__(1)));

/**
 * @attention this function must be treated as opaque therefore must not be called directly.
 */
extern Option_Value __Option_expect(const char *file, int line, Option self, const char *format, ...)
__attribute__((__nonnull__(1, 4), __format__(__printf__, 4, 5)));

#ifdef __cplusplus
}
#endif
