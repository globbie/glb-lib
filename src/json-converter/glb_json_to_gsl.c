#include "glb-lib/json_converter.h"

#include "glb-lib/config.h"

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
    int err = glb_OK;
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
                err = output->writec(output, '[');
                if (err) goto out;
                err = output->write(output, tag, tag_size);
                if (err) goto out;
                err = output->writec(output, ' ');
                if (err) goto out;
            }
            c++;
            err = process_task(&c, rec_end, in_change, NULL, 0, output);
            if (err) goto out;
            break;
        case ',':
            err = output->writec(output, ' ');
            if (err) goto out;
            c++;
            err = process_task(&c, rec_end, in_change, NULL, 0, output);
            if (err) goto out;
            break;
        case ']':
            if (!in_place)
                err = output->writec(output, ']');
            goto out;
        default:
            err = glb_FORMAT;
            goto out;
        }
    }

out:
    *rec = c;
    return err;
}

static int
process_task(const char **rec, const char *rec_end,
             bool in_change, const char *tag, size_t tag_size,
             struct glbOutput *output)
{
    int err = glb_OK;
    bool in_place = tag_size == 0;

    bool in_tag = false;
    bool in_value = false;
    bool in_terminal_value = false;

    const char *c = *rec;
    for (; c < rec_end; c++) {
        if (in_tag && in_value) { err = glb_FORMAT; goto out; }
        if (in_terminal_value) assert(in_value);

        switch (*c) {
        case '\t':
        case '\n':
        case '\r':
        case ' ':
            if (in_tag) { err = glb_FORMAT; goto out; }  // error: tag contains spaces
            if (in_terminal_value) {
                err = output->writec(output, *c);
                if (err) goto out;
            }
            break;
        case '{': {
            if (!in_value) {
                // Starting opening \{ brace.  Ignore it or print the passed |tag|
                if (!in_place) {
                    err = output->writec(output, !in_change ? '{' : '(');
                    if (err) goto out;
                    err = output->write(output, tag, tag_size);
                    if (err) goto out;
                    err = output->writec(output, ' ');
                    if (err) goto out;
                }
                continue;
            }

            if (in_tag) { err = glb_FORMAT; goto out; }
            assert(in_value == true);
            if (in_terminal_value) { err = glb_FORMAT; goto out; }

            const bool tag_is_set = tag_size == strlen("__set__") && !memcmp(tag, "__set__", tag_size);
            if (tag_is_set && in_change) { err = glb_FORMAT; goto out; }  // error: duplicated __set__
            err = process_task(&c, rec_end, !tag_is_set ? in_change : true,
                              !tag_is_set ? tag : NULL, !tag_is_set ? tag_size : 0,
                              output);
            if (err) goto out;
            // in_tag == false
            in_value = false;
            // in_terminal == false
            break;
        }
        case '[': {
            if (in_tag) { err = glb_FORMAT; goto out; }
            if (!in_value) { err = glb_FORMAT; goto out; }
            if (in_terminal_value) { err = glb_FORMAT; goto out; }

            const bool tag_is_seq = tag_size == strlen("__seq__") && !memcmp(tag, "__seq__", tag_size);
            err = process_task_array(&c, rec_end, in_change,
                                     !tag_is_seq ? tag : NULL,
                                     !tag_is_seq ? tag_size : 0,
                                     output);
            if (err) goto out;
            // in_tag == false
            in_value = false;
            // in_terminal_value == false
            break;
        }
        case '"':
            if (!in_tag && !in_value) {
                if (in_terminal_value) { err = glb_FORMAT; goto out; }

                tag = c + 1;
                tag_size = 0;
                in_tag = true;
                // in_value == false
                // in_terminal_value == false
            }
            else if (in_tag) {
                if (in_value) { err = glb_FORMAT; goto out; }
                if (in_terminal_value) { err = glb_FORMAT; goto out; }

                // |tag| contains the field's tag
                in_tag = false;
                // in_value == false
                // in_terminal_value == false
            }
            else if (in_value && !in_terminal_value) {
                assert(in_tag == false);

                err = output->writec(output, !in_change ? '{' : '(');
                if (err) goto out;
                err = output->write(output, tag, tag_size);
                if (err) goto out;
                err = output->writec(output, ' ');
                if (err) goto out;
                // in_tag == false
                // in_value == true
                in_terminal_value = true;
            }
            else if (in_value && in_terminal_value) {
                if (in_tag) { err = glb_FORMAT; goto out; }

                err = output->writec(output, !in_change ? '}' : ')');
                if (err) goto out;
                // in_tag == false
                in_value = false;
                in_terminal_value = false;
            }
            break;
        case ':':
            if (in_tag) { err = glb_FORMAT; goto out; }
            if (in_value) { err = glb_FORMAT; goto out; }
            if (in_terminal_value) { err = glb_FORMAT; goto out; }

            in_value = true;
            break;
        case ',':
            if (in_tag) { err = glb_FORMAT; goto out; }
            if (in_value) { err = glb_FORMAT; goto out; }
            if (in_terminal_value) { err = glb_FORMAT; goto out; }

            err = output->writec(output, ' ');
            if (err) goto out;
            break;
        case '}':
            if (in_tag) { err = glb_FORMAT; goto out; }
            if (in_value) { err = glb_FORMAT; goto out; }
            if (in_terminal_value) { err = glb_FORMAT; goto out; }

            if (!in_place)
                err = output->writec(output, !in_change ? '}' : ')');
            goto out;
        default:
            if (in_tag)
                tag_size++;
            else if (in_terminal_value) {
                err = output->writec(output, *c);
                if (err) goto out;
            }
            else {
                err = glb_FORMAT;
                goto out;
            }
            break;
        }
    }

out:
    *rec = c;
    return err;
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
