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

#include <assert.h>
#include <stddef.h>
#include <panic/panic.h>
#include "option.h"

const OptionView NoneView = {.__value=NULL, .__variant=OptionVariant_None};

OptionView OptionView_some(const OptionView_Value value) {
    return (OptionView) {.__value=value, .__variant=OptionVariant_Some};
}

OptionView OptionView_fromNullable(const OptionView_Value value) {
    return NULL == value ? NoneView : OptionView_some(value);
}

bool OptionView_isNone(const OptionView self) {
    return OptionVariant_None == self.__variant;
}

bool OptionView_isSome(const OptionView self) {
    return OptionVariant_Some == self.__variant;
}

OptionView OptionView_map(const OptionView self, OptionView (*const f)(OptionView_Value)) {
    assert(NULL != f);
    return OptionView_isNone(self) ? NoneView : f(OptionView_unwrap(self));
}

OptionView OptionView_alt(const OptionView self, const OptionView a) {
    return OptionView_isNone(self) ? a : self;
}

OptionView OptionView_chain(const OptionView self, OptionView (*const f)(OptionView_Value)) {
    assert(NULL != f);
    return OptionView_isNone(self) ? NoneView : f(OptionView_unwrap(self));
}

OptionView_Value
OptionView_fold(const OptionView self, OptionView_Value (*const whenNone)(void),
                OptionView_Value (*const whenSome)(OptionView_Value)) {
    assert(NULL != whenNone);
    assert(NULL != whenSome);
    return OptionView_isNone(self) ? whenNone() : whenSome(OptionView_unwrap(self));
}

OptionView_Value OptionView_getOr(const OptionView self, const OptionView_Value defaultValue) {
    return OptionView_isNone(self) ? defaultValue : OptionView_unwrap(self);
}

OptionView_Value OptionView_getOrElse(const OptionView self, OptionView_Value (*const f)(void)) {
    assert(NULL != f);
    return OptionView_isNone(self) ? f() : OptionView_unwrap(self);
}

OptionView_Value __OptionView_unwrap(const char *const file, const int line, const OptionView self) {
    assert(NULL != file);
    assert(line > 0);
    if (OptionView_isNone(self)) {
        __Panic_terminate(file, line, "%s", "Unable to unwrap value");
    }
    return self.__value;
}

OptionView_Value
__OptionView_expect(const char *const file, const int line, const OptionView self, const char *const format, ...) {
    assert(NULL != file);
    assert(line > 0);
    assert(NULL != format);
    if (OptionView_isNone(self)) {
        va_list args;
        va_start(args, format);
        __Panic_vterminate(file, line, format, args);
    }
    return self.__value;
}

const Option None = {.__value=NULL, .__variant=OptionVariant_None};

Option Option_some(const Option_Value value) {
    return (Option) {.__value=value, .__variant=OptionVariant_Some};
}

Option Option_fromNullable(const Option_Value value) {
    return NULL == value ? None : Option_some(value);
}

OptionView Option_toView(const Option self) {
    return (OptionView) {.__value=self.__value, .__variant=self.__variant};
}

bool Option_isNone(const Option self) {
    return OptionVariant_None == self.__variant;
}

bool Option_isSome(const Option self) {
    return OptionVariant_Some == self.__variant;
}

Option Option_map(const Option self, Option (*const f)(Option_Value)) {
    assert(NULL != f);
    return Option_isNone(self) ? None : f(Option_unwrap(self));
}

Option Option_alt(const Option self, const Option a) {
    return Option_isNone(self) ? a : self;
}

Option Option_chain(const Option self, Option (*const f)(Option_Value)) {
    assert(NULL != f);
    return Option_isNone(self) ? None : f(Option_unwrap(self));
}

Option_Value
Option_fold(const Option self, Option_Value (*const whenNone)(void), Option_Value (*const whenSome)(Option_Value)) {
    assert(NULL != whenNone);
    assert(NULL != whenSome);
    return Option_isNone(self) ? whenNone() : whenSome(Option_unwrap(self));
}

Option_Value Option_getOr(const Option self, const Option_Value defaultValue) {
    return Option_isNone(self) ? defaultValue : Option_unwrap(self);
}

Option_Value Option_getOrElse(const Option self, Option_Value (*const f)(void)) {
    assert(NULL != f);
    return Option_isNone(self) ? f() : Option_unwrap(self);
}

Option_Value __Option_unwrap(const char *const file, const int line, const Option self) {
    assert(NULL != file);
    assert(line > 0);
    if (Option_isNone(self)) {
        __Panic_terminate(file, line, "%s", "Unable to unwrap value");
    }
    return self.__value;
}

Option_Value __Option_expect(const char *const file, const int line, const Option self, const char *const format, ...) {
    assert(NULL != file);
    assert(line > 0);
    assert(NULL != format);
    if (Option_isNone(self)) {
        va_list args;
        va_start(args, format);
        __Panic_vterminate(file, line, format, args);
    }
    return self.__value;
}
