#pragma once

#include "config.h"

#include <stddef.h>

#define GLB_OUTPUT_THRESHOLD_RATIO 0.8

struct glbOutput
{
    char *buf;
    size_t buf_size;
    size_t capacity;
    size_t threshold;

    /**********  interface methods  **********/
    void (*del)(struct glbOutput *self);

    void (*reset)(struct glbOutput *self);

    int (*rtrim)(struct glbOutput *self,
                 size_t trim_size);

    int (*write)(struct glbOutput *self,
                 const char *buf,
                 size_t buf_size);
    int (*writec)(struct glbOutput *self,
                  char ch);
    int (*writef)(struct glbOutput *self,
                  const char *format,
                  ...);

    int (*write_escaped)(struct glbOutput *self,
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
