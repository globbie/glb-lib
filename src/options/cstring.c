#include <glb-lib/options.h>

#include <string.h>
#include <stdio.h>

static int
parse__(struct glbOption *self, const char *input, size_t input_len)
{
    self->data = malloc(input_len + 1);
    if (!self->data) return -1;

    memcpy(self->data, input, input_len);
    ((char *) self->data)[input_len] = 0;

    return 0;
}

static int
free__(struct glbOption *self)
{
    if (self->data) free(self->data);
    return 0;
}

static int
print__(struct glbOption *self)
{
    printf("\t--%s=%s\n", self->name, (char *) self->data);
    return 0;
}

struct glbOptType kndCStringOptType = {
    .name = "C-STRING",
    .has_arg = true,
    .init = NULL,
    .parse = parse__,
    .free = free__,
    .print = print__
};
