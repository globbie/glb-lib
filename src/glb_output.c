#include "glb-lib/output.h"
#include "glb-lib/log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG_OUTPUT_LEVEL_0 0
#define DEBUG_OUTPUT_LEVEL_1 0
#define DEBUG_OUTPUT_LEVEL_2 0
#define DEBUG_OUTPUT_LEVEL_3 0
#define DEBUG_OUTPUT_LEVEL_TMP 1

static void
glbOutput_del(struct glbOutput *self)
{
    free(self->buf);
    free(self);
}

static void
glbOutput_reset(struct glbOutput *self)
{
    self->buf_size = 0;
    self->buf[self->buf_size] = '\0';
}

static int
glbOutput_rtrim(struct glbOutput *self,
                size_t trim_size)
{
    if (trim_size > self->buf_size) return glb_LIMIT;

    self->buf_size -= trim_size;
    self->buf[self->buf_size] = '\0';

    return glb_OK;
}

static int
glbOutput_writec(struct glbOutput *self,
                 char ch)
{
    if (self->buf_size >= self->capacity - 1)
        return glb_NOMEM;

    self->buf[self->buf_size] = ch;
    self->buf_size++;
    self->buf[self->buf_size] = '\0';
    return glb_OK;
}

static int
glbOutput_write(struct glbOutput *self,
                const char *buf,
                size_t buf_size)
{
    if (buf_size > self->capacity - self->buf_size - 1)
        return glb_NOMEM;

    memcpy(self->buf + self->buf_size, buf, buf_size);
    self->buf_size += buf_size;
    self->buf[self->buf_size] = '\0';
    return glb_OK;
}

static int
glbOutput_write_escaped(struct glbOutput *self,
                        const char *buf,
                        size_t buf_size)
{
    size_t free_space = self->capacity - self->buf_size;
    size_t chunk_size = 0;
    char *c = self->buf + self->buf_size;
    const char *b;

    for (size_t i = 0; i < buf_size; i++) {
        b = buf + i;
        if (chunk_size >= free_space)
            return glb_NOMEM;

        switch (*b) {
        case '{':
        case '}':
        case '"':
        case '\'':
            break;
        default:
            *c = *b;
            c++;
            chunk_size++;
        }
    }
    
    self->buf_size += chunk_size;
    return glb_OK;
}

static int
glbOutput_write_state_path(struct glbOutput *self,
                           const char *state,
                           size_t state_size)
{
    size_t rec_size = state_size * 2 + 1;
    if (rec_size > self->capacity - self->buf_size)
        return glb_NOMEM;

    char *rec = self->buf + self->buf_size;

    for (const char *state_end = state + state_size; state < state_end; state++) {
        *rec++ =  '/';
        *rec++ = *state;
    }
    *rec = '\0';

    self->buf_size += rec_size;

    return glb_OK;
}

static int
glbOutput_write_file_content(struct glbOutput *self,
                             const char *file_name/*,
                             bool allow_resize*/)
{
    int err;
    FILE *file_stream;
    long file_size;
    size_t read_size;

    if (DEBUG_OUTPUT_LEVEL_2)
        glb_log(".. IO [%p] reading the \"%s\" file..", self, file_name);

    if (self->buf_size != 0) return glb_FAIL;

    if (DEBUG_OUTPUT_LEVEL_3)
        glb_log(" .. opening file \"%s\"..\n", file_name);

    file_stream = fopen(file_name, "r");
    if (file_stream == NULL) {
        glb_log("-- error opening FILE \"%s\"",
                file_name);
        return glb_IO_FAIL;
    }

    if (fseek(file_stream, 0L, SEEK_END) != 0) { err = glb_IO_FAIL; goto final; }

    file_size = ftell(file_stream);
    if (file_size == -1) { err = glb_IO_FAIL; goto final; }

    if (fseek(file_stream, 0L, SEEK_SET) != 0) { err = glb_IO_FAIL; goto final; }

    if ((size_t)file_size > self->capacity - self->buf_size - 1) { err = glb_NOMEM; goto final; }

    if (DEBUG_OUTPUT_LEVEL_3)
        glb_log("  .. reading FILE \"%s\" [%ld] ...\n",
                file_name, file_size);

    read_size = fread(self->buf + self->buf_size, 1, file_size, file_stream);
    self->buf_size += read_size;
    self->buf[self->buf_size] = '\0';

    if (DEBUG_OUTPUT_LEVEL_3)
        glb_log("   ++ FILE \"%s\" read OK [size: %zu]\n",
                file_name, read_size);

    err = glb_OK;

final:

    fclose(file_stream);

    return err;
}


static int
glbOutput_init(struct glbOutput *self,
               size_t capacity)
{
    /* output buffer */
    self->buf = malloc(capacity * sizeof(char));
    if (self->buf == NULL) return glb_NOMEM;

    self->buf_size = 0;
    self->capacity = capacity;

    self->del = glbOutput_del;
    self->reset = glbOutput_reset;
    self->rtrim = glbOutput_rtrim;
    self->writec = glbOutput_writec;
    self->write = glbOutput_write;
    self->write_escaped = glbOutput_write_escaped;
    self->write_state_path = glbOutput_write_state_path;
    self->write_file_content = glbOutput_write_file_content;

    return glb_OK;
}

int
glbOutput_new(struct glbOutput **output,
              size_t capacity)
{
    int err;
    struct glbOutput *self;

    self = malloc(sizeof(struct glbOutput));
    if (!self) return glb_NOMEM;

    err = glbOutput_init(self, capacity);
    if (err != 0) {
        free(self);
        return err;
    }

    self->threshold = capacity * GLB_OUTPUT_THRESHOLD_RATIO;

    *output = self;

    return glb_OK;
}
