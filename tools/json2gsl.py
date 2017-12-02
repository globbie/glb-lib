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


def process_task_array(in_change = False, tag = None):
    in_place = tag == None

    while True:
        ch = getc()
        if ch == '': sys.exit(1)

        if ch.isspace():
            pass
        elif ch == '[':
            if not in_place:
                putc('[')
                for ch_ in tag: putc(ch_)
                putc(' ')
            process_task(in_change = in_change, tag = None)
        elif ch == ',':
            putc(' ')
            process_task(in_change = in_change, tag = None)
        elif ch == ']':
            if not in_place:
                putc(']')
            return

def process_task(in_change = False, tag = None):
    in_place = tag == None

    in_tag = False
    in_value = False
    in_terminal_value = False

    while True:
        ch = getc()
        if ch == '': sys.exit(0)

        assert not in_tag or not in_value
        if in_terminal_value: assert in_value

        if ch.isspace():
            assert not in_tag, "error: tag contains spaces"
            if in_terminal_value:
                putc(ch)
        elif ch == '{':
            if not in_value:
                # Starting opening { brace.  Ignore it or print the passed |tag|
                if not in_place:
                    putc('{' if not in_change else '(')
                    for ch_ in tag: putc(ch_)
                    putc(' ')
                continue
            ungetc('{')
            assert tag != '__set__' or not in_change
            process_task(in_change = in_change if tag != '__set__' else True,
                         tag = tag if tag != '__set__' else None)
            assert in_tag == False
            assert in_value == True; in_value = False
            assert in_terminal_value == False
        elif ch == '[':
            ungetc('[')
            process_task_array(in_change = in_change,
                               tag = tag if tag != '__seq__' else None)
            assert in_tag == False
            assert in_value == True; in_value = False
            assert in_terminal_value == False
        elif ch == '"':
            if not in_tag and not in_value:
                tag = ""
                in_tag = True
                # in_value == False
                assert in_terminal_value == False
            elif in_tag:
                # |tag| contains the field's tag
                in_tag = False
                assert in_value == False
                assert in_terminal_value == False
            elif in_value and not in_terminal_value:
                putc('{' if not in_change else '(')
                for ch_ in tag: putc(ch_)
                putc(' ')
                assert in_tag == False
                # in_value == True
                in_terminal_value = True
            elif in_value and in_terminal_value:
                putc('}' if not in_change else ')')
                assert in_tag == False
                in_value = False
                in_terminal_value = False
        elif ch == ':':
            assert in_tag == False
            assert in_value == False; in_value = True
            assert in_terminal_value == False
        elif ch == ',':
            putc(' ')
            assert in_tag == False
            assert in_value == False
            assert in_terminal_value == False
        elif ch == '}':
            if not in_place:
                putc('}' if not in_change else ')')
            assert in_tag == False
            assert in_value == False
            assert in_terminal_value == False
            return
        else:
            if in_tag:
                tag += ch
            elif in_terminal_value:
                putc(ch)

process_task()

sys.stdout.flush()

