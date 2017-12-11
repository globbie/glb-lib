#include "glb_json_converter.h"

#include "glb_config.h"

#include <assert.h>
#include <stddef.h>
#include <string.h>

static int process_task(const char **rec, const char *rec_end,
                        bool in_change, const char *tag, size_t tag_size,
                        struct glbOutput *output);

static int
process_task_array(const char **rec, const char *rec_end,
                   bool in_change, const char *tag, size_t tag_size,
                   struct glbOutput *output)
{
    bool in_place = tag_size == 0;

    const char *c = *rec;
    for (; c < rec_end; c++) {
        switch (*c) {
        case '\t':
        case '\n':
        case '\r':
        case ' ':
            break;
        case '[':
            if (!in_place) {
                output->putc(output, '[');
                output->write(output, tag, tag_size);
                output->putc(output, ' ');
            }
            c++;
            process_task(&c, rec_end, in_change, NULL, 0, output);
            break;
        case ',':
            output->putc(output, ' ');
            c++;
            process_task(&c, rec_end, in_change, NULL, 0, output);
            break;
        case ']':
            if (!in_place)
                output->putc(output, ']');
            goto out;
        }
    }

out:
    *rec = c;
    return 0;
}

static int
process_task(const char **rec, const char *rec_end,
             bool in_change, const char *tag, size_t tag_size,
             struct glbOutput *output)
{
    bool in_place = tag_size == 0;

    bool in_tag = false;
    bool in_value = false;
    bool in_terminal_value = false;

    const char *c = *rec;
    for (; c < rec_end; c++) {
        assert(!in_tag || !in_value);
        if (in_terminal_value) assert(in_value);

        switch (*c) {
        case '\t':
        case '\n':
        case '\r':
        case ' ':
            assert(!in_tag && "error: tag contains spaces");
            if (in_terminal_value)
                output->putc(output, *c);
            break;
        case '{': {
            if (!in_value) {
                // Starting opening \{ brace.  Ignore it or print the passed |tag|
                if (!in_place) {
                    output->putc(output, !in_change ? '{' : '(');
                    output->write(output, tag, tag_size);
                    output->putc(output, ' ');
                }
                continue;
            }
            bool tag_is_set = tag_size == strlen("__set__") && !memcmp(tag, "__set__", tag_size);
            assert(!tag_is_set || !in_change);
            process_task(&c, rec_end, !tag_is_set ? in_change : true,
                         !tag_is_set ? tag : NULL, !tag_is_set ? tag_size : 0,
                         output);
            assert(in_tag == false);
            assert(in_value == true); in_value = false;
            assert(in_terminal_value == false);
            break;
        }
        case '[': {
            bool tag_is_seq = tag_size == strlen("__seq__") && !memcmp(tag, "__seq__", tag_size);
            process_task_array(&c, rec_end, in_change,
                               !tag_is_seq ? tag : NULL,
                               !tag_is_seq ? tag_size : 0,
                               output);
            assert(in_tag == false);
            assert(in_value == true); in_value = false;
            assert(in_terminal_value == false);
            break;
        }
        case '"':
            if (!in_tag && !in_value) {
                tag = c + 1;
                tag_size = 0;
                in_tag = true;
                // in_value == false
                assert(in_terminal_value == false);
            }
            else if (in_tag) {
                // |tag| contains the field's tag
                in_tag = false;
                assert(in_value == false);
                assert(in_terminal_value == false);
            }
            else if (in_value && !in_terminal_value) {
                output->putc(output, !in_change ? '{' : '(');
                output->write(output, tag, tag_size);
                output->putc(output, ' ');
                assert(in_tag == false);
                // in_value == true
                in_terminal_value = true;
            }
            else if (in_value && in_terminal_value) {
                output->putc(output, !in_change ? '}' : ')');
                assert(in_tag == false);
                in_value = false;
                in_terminal_value = false;
            }
            break;
        case ':':
            assert(in_tag == false);
            assert(in_value == false); in_value = true;
            assert(in_terminal_value == false);
            break;
        case ',':
            output->putc(output, ' ');
            assert(in_tag == false);
            assert(in_value == false);
            assert(in_terminal_value == false);
            break;
        case '}':
            if (!in_place)
                output->putc(output, !in_change ? '}' : ')');
            assert(in_tag == false);
            assert(in_value == false);
            assert(in_terminal_value == false);
            goto out;
        default:
            if (in_tag)
                tag_size++;
            else if (in_terminal_value)
                output->putc(output, *c);
            break;
        }
    }

out:
    *rec = c;
    return 0;
}

int
glb_json_to_gsl(const char *json, size_t json_size, size_t *handled, struct glbOutput *output)
{
    const char *c = json;
    int err = process_task(&c, json + json_size, false, NULL, 0, output);
    if (handled != NULL)
        *handled = c - json;
    return err;
}
