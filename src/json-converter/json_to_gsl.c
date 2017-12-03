#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include <ctype.h>
#include <stdlib.h>

int glb_json_to_gsl(const char *json, size_t json_size, char *out, size_t *out_size) {
  return 0;
}

int process_task(const char **rec, const char *rec_end,
                 bool in_change, const char *tag, size_t tag_size);

static int process_task_array(const char **rec, const char *rec_end,
                              bool in_change, const char *tag, size_t tag_size) {
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
                putc('[', stdout);
                for (size_t i = 0; i < tag_size; ++i) putc(tag[i], stdout);
                putc(' ', stdout);
            }
            c++;
            process_task(&c, rec_end, in_change, NULL, 0);
            break;
        case ',':
            putc(' ', stdout);
            c++;
            process_task(&c, rec_end, in_change, NULL, 0);
            break;
        case ']':
            if (!in_place)
                putc(']', stdout);
            goto out;
        }
    }

out:
    *rec = c;
    return 0;
}

int process_task(const char **rec, const char *rec_end,
                 bool in_change, const char *tag, size_t tag_size) {
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
                putc(*c, stdout);
            break;
        case '{': {
            if (!in_value) {
                // Starting opening \{ brace.  Ignore it or print the passed |tag|
                if (!in_place) {
                    putc(!in_change ? '{' : '(', stdout);
                    for (size_t i = 0; i < tag_size; ++i) putc(tag[i], stdout);
                    putc(' ', stdout);
                }
                continue;
            }
            bool tag_is_set = tag_size == strlen("__set__") && !memcmp(tag, "__set__", tag_size);
            assert(!tag_is_set || !in_change);
            process_task(&c, rec_end, !tag_is_set ? in_change : true,
                         !tag_is_set ? tag : NULL, !tag_is_set ? tag_size : 0);
            assert(in_tag == false);
            assert(in_value == true); in_value = false;
            assert(in_terminal_value == false);
            break;
        }
        case '[': {
            bool tag_is_seq = tag_size == strlen("__seq__") && !memcmp(tag, "__seq__", tag_size);
            process_task_array(&c, rec_end, in_change,
                               !tag_is_seq ? tag : NULL,
                               !tag_is_seq ? tag_size : 0);
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
                putc(!in_change ? '{' : '(', stdout);
                for (size_t i = 0; i < tag_size; ++i) putc(tag[i], stdout);
                putc(' ', stdout);
                assert(in_tag == false);
                // in_value == true
                in_terminal_value = true;
            }
            else if (in_value && in_terminal_value) {
                putc(!in_change ? '}' : ')', stdout);
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
            putc(' ', stdout);
            assert(in_tag == false);
            assert(in_value == false);
            assert(in_terminal_value == false);
            break;
        case '}':
            if (!in_place)
                putc(!in_change ? '}' : ')', stdout);
            assert(in_tag == false);
            assert(in_value == false);
            assert(in_terminal_value == false);
            goto out;
        default:
            if (in_tag)
                tag_size++;
            else if (in_terminal_value)
                putc(*c, stdout);
            break;
        }
    }

out:
    *rec = c;
    return 0;
}

int main () {
    char buf[1000];

    const char *json = buf;
    size_t json_size = fread(buf, 1, sizeof buf, stdin);
    process_task(&json, json + json_size, false, NULL, 0);
    return 0;
}

