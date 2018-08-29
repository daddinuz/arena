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

#include <stdio.h>
#include <arena.h>
#include <string.h>
#include <assert.h>
#include <stdalign.h>

static void printArena(const struct Arena *const arena) {
    printf("Arena(size=%zu, slop=%zu, capacity=%zu, bestAvailable=%zu, worstAvailable=%zu, chunks=%zu)\n",
           Arena_getSize(arena), Arena_getSlop(arena), Arena_getCapacity(arena), Arena_getBestAvailable(arena),
           Arena_getWorstAvailable(arena), Arena_getChunks(arena));
}

static char *String_new(struct Arena *const arena, const char *const literal) {
    assert(arena);
    assert(literal);
    const size_t size = strlen(literal);
    char *self = Arena_requestWithAlignment(arena, alignof(char), size + 1);
    strncpy(self, literal, size);
    self[size] = 0;
    return self;
}

struct Person {
    char *firstName;
    char *lastName;
};

static struct Person *Person_new(struct Arena *const arena, const char *const firstName, const char *const lastName) {
    assert(arena);
    assert(firstName);
    assert(lastName);
    struct Person *self = Arena_request(arena, sizeof(*self));
    self->firstName = String_new(arena, firstName);
    self->lastName = String_new(arena, lastName);
    return self;
}

static void Person_print(const struct Person *const self) {
    assert(self);
    printf("Person(firstName=\"%s\", lastName=\"%s\")\n", self->firstName, self->lastName);
}

int main() {
    struct Arena *arena = Arena_new();
    printArena(arena);

    struct Person *giuseppeGaribaldi = Person_new(arena, "Giuseppe", "Garibaldi");
    printArena(arena);

    struct Person *vittorioEmanuele = Person_new(arena, "Vittorio", "Emanuele");
    printArena(arena);

    Person_print(giuseppeGaribaldi);
    Person_print(vittorioEmanuele);

    Arena_clear(arena);
    printArena(arena);

    struct Person *camilloBenso = Person_new(arena, "Camillo", "Benso");
    printArena(arena);

    Person_print(camilloBenso);

    Arena_delete(arena);
    return 0;
}
