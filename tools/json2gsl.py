#!/usr/bin/env python3

import sys

ungotten = None

def getc():
    global ungotten
    if ungotten == None: return sys.stdin.read(1)
    ch = ungotten
    ungotten = None
    return ch

def ungetc(ch):
    global ungotten
    assert ungotten == None
    ungotten = ch

def putc(ch):
    sys.stdout.write(ch)


def process_task():
    in_field = False
    in_tag = False
    in_value = False
    in_terminal_value = False

    __set__ = -1

    while True:
        ch = getc()
        if ch == '': sys.exit(0)

        if ch.isspace():
            assert not in_tag, "error: tag contains spaces"
            if in_terminal_value:
                putc(ch)
        elif ch == '{':
            if not in_field:
                #putc('{')
                in_field = True
                assert in_tag == False
                assert in_value == False
                assert in_terminal_value == False
                continue
            assert in_field and not in_tag and in_value and not in_terminal_value
            ungetc('{')
            process_task()
            putc('}')
            in_field = False
            assert in_tag == False
            in_value = False
            assert in_terminal_value == False
        elif ch == '"':
            if in_field and not in_tag and not in_value:
                # in_field == True
                in_tag = True
                # in_value == False
                assert in_terminal_value == False
            elif in_field and in_tag:
                putc(' ')
                # in_field == True
                in_tag = False
                assert in_value == False
                assert in_terminal_value == False
            elif in_field and in_value and not in_terminal_value:
                # in_field == True
                assert in_tag == False
                # in_value == True
                in_terminal_value = True
            elif in_field and in_value and in_terminal_value:
                putc('}')
                in_field = False
                assert in_tag == False
                in_value = False
                in_terminal_value = False
        elif ch == ':':
            assert in_field == True
            assert in_tag == False
            assert in_value == False; in_value = True
            assert in_terminal_value == False
        elif ch == ',':
            putc(' '); putc('{')
            assert in_field == False; in_field = True
            assert in_tag == False
            assert in_value == False
            assert in_terminal_value == False
        elif ch == '}':
            assert in_field == False
            assert in_tag == False
            assert in_value == False
            assert in_terminal_value == False
            return
        else:
            if in_tag or in_terminal_value:
                putc(ch)

process_task()

sys.stdout.flush()

