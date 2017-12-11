#pragma once

#include "glb_config.h"

#include <stddef.h>

struct glbOutput
{
    char *buf;
    size_t buf_size;
    size_t capacity;

    /**********  interface methods  **********/
    void (*del)(struct glbOutput *self);

    void (*reset)(struct glbOutput *self);

    int (*rtrim)(struct glbOutput *self,
                 size_t trim_size);

    int (*putc)(struct glbOutput *self,
                char ch);

    int (*write)(struct glbOutput *self,
                 const char *buf,
                 size_t buf_size);

    int (*write_state_path)(struct glbOutput *self,
                            const char *state,
                            size_t state_size);

    int (*write_file_content)(struct glbOutput *self,
                              const char *filename);
};

extern int glbOutput_new(struct glbOutput **self,
                         size_t capacity);
