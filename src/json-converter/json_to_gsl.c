#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include <ctype.h>
#include <stdlib.h>

int glb_json_to_gsl(const char *json, size_t json_size, char *out, size_t *out_size) {
  return 0;
}

int process_task(bool in_change, char *tag, size_t tag_size);

static int process_task_array(bool in_change, const char *tag, size_t tag_size) {
    bool in_place = tag_size == 0;

    while (1) {
        int ch = getc(stdin);
        if (ch == EOF) exit(1);

        if (isspace(ch)) {
            // nothing
        }
        else if (ch == '[') {
            if (!in_place) {
                putc('[', stdout);
                for (size_t i = 0; i < tag_size; ++i) putc(tag[i], stdout);
                putc(' ', stdout);
            }
            process_task(in_change, NULL, 0);
        }
        else if (ch == ',') {
            putc(' ', stdout);
            process_task(in_change, NULL, 0);
        }
        else if (ch == ']') {
            if (!in_place)
                putc(']', stdout);
            return 0;
        }
    }
}

int process_task(bool in_change, char *tag, size_t tag_size) {
    char buf[1000];
    bool in_place = tag_size == 0;

    bool in_tag = false;
    bool in_value = false;
    bool in_terminal_value = false;

    while (1) {
        int ch = getc(stdin);
        if (ch == EOF) exit(0);

        assert(!in_tag || !in_value);
        if (in_terminal_value) assert(in_value);

        if (isspace(ch)) {
            assert(!in_tag && "error: tag contains spaces");
            if (in_terminal_value)
                putc(ch, stdout);
        }
        else if (ch == '{') {
            if (!in_value) {
                // Starting opening { brace.  Ignore it or print the passed |tag|
                if (!in_place) {
                    putc(!in_change ? '{' : '(', stdout);
                    for (size_t i = 0; i < tag_size; ++i) putc(tag[i], stdout);
                    putc(' ', stdout);
                }
                continue;
            }
            ungetc('{', stdin);
            bool tag_is_set = tag_size == strlen("__set__") && !memcmp(tag, "__set__", tag_size);
            assert(!tag_is_set || !in_change);
            process_task(!tag_is_set ? in_change : true,
                         !tag_is_set ? tag : NULL,
                         !tag_is_set ? tag_size : 0);
            assert(in_tag == false);
            assert(in_value == true); in_value = false;
            assert(in_terminal_value == false);
        }
        else if (ch == '[') {
            ungetc('[', stdin);
            bool tag_is_seq = tag_size == strlen("__seq__") && !memcmp(tag, "__seq__", tag_size);
            process_task_array(in_change,
                               !tag_is_seq ? tag : NULL,
                               !tag_is_seq ? tag_size : 0);
            assert(in_tag == false);
            assert(in_value == true); in_value = false;
            assert(in_terminal_value == false);
        }
        else if (ch == '"') {
            if (!in_tag && !in_value) {
                tag = buf;
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
        }
        else if (ch == ':') {
            assert(in_tag == false);
            assert(in_value == false); in_value = true;
            assert(in_terminal_value == false);
        }
        else if (ch == ',') {
            putc(' ', stdout);
            assert(in_tag == false);
            assert(in_value == false);
            assert(in_terminal_value == false);
        }
        else if (ch == '}') {
            if (!in_place)
                putc(!in_change ? '}' : ')', stdout);
            assert(in_tag == false);
            assert(in_value == false);
            assert(in_terminal_value == false);
            return 0;
        }
        else {
            if (in_tag)
                tag[tag_size++] = (char)ch;
            else if(in_terminal_value)
                putc(ch, stdout);
        }
    }
}

int main () {
    process_task(false, NULL, 0);
    return 0;
}

