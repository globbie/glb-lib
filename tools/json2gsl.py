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


def process_task(in_change = False):
    set_tag = "__set__"
    set_state = -1
    seq_tag = "__seq__"
    seq_state = -1

    in_field = False
    in_tag = False
    in_value = False
    in_terminal_value = False

    while True:
        ch = getc()
        if ch == '': sys.exit(0)

        if ch.isspace():
            assert not in_tag, "error: tag contains spaces"
            if in_terminal_value:
                putc(ch)
        elif ch == '{':
            if not in_field:
                in_field = True
                assert in_tag == False
                assert in_value == False
                assert in_terminal_value == False
                continue
            assert in_field and not in_tag and in_value and not in_terminal_value
            ungetc('{')
            process_task(in_change or (set_state == len(set_tag)))
            if set_state != len(set_tag): putc('}' if not in_change else ')')
            in_field = False
            assert in_tag == False
            in_value = False
            assert in_terminal_value == False
        elif ch == '"':
            if in_field and not in_tag and not in_value:
                set_state = 0; seq_state = 0
                # in_field == True
                in_tag = True
                # in_value == False
                assert in_terminal_value == False
            elif in_field and in_tag:
                if set_state != len(set_tag) and seq_state != len(seq_tag): putc(' ')
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
                putc('}' if not in_change else ')')
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
            putc(' ')
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
            if in_tag:
                if set_state != -1 or seq_state != -1:
                    pending = None
                    print_pending = True
                    if set_state != -1:
                        if set_state < len(set_tag) and ch == set_tag[set_state]:
                            pending = None
                            print_pending = False
                            set_state += 1
                        else:
                            pending = set_tag[:set_state]
                            set_state = -1
                    if seq_state != -1:
                        if seq_state < len(seq_tag) and ch == seq_tag[seq_state]:
                            pending = None
                            print_pending = False
                            seq_state += 1
                        else:
                            pending = seq_tag[:seq_state]
                            seq_state = -1
                    if print_pending and pending != None:
                        putc('{' if not in_change else '(')
                        for ch_ in pending: putc(ch_)
                        putc(ch) # priint
                else:
                    putc(ch)
            elif in_terminal_value:
                putc(ch)

process_task()

sys.stdout.flush()

