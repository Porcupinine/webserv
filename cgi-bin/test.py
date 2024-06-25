#!/usr/bin/env python3

import os
import sys
import cgi
import cgitb
import os
import cgitb
cgitb.enable()

def do_my_stuff():
    # for line in sys.stdin:
    #     print(f"bode line: {line}\n")
    # print(f"my env = {os.environ}")
    # print(f"my args = {sys.argv} ")

    if not os.path.isdir("logdir"):
        os.mkdir("logdir")
    if not os.path.isdir("uploads"):
        os.mkdir("uploads")
    cgitb.enable(display=0, logdir="logdir")  # for debuging
    form = cgi.FieldStorage()
    # filename = form.getvalue('filename')
    print(f"headers: {form.headers}")
    print(f"filename: {form.filename}\n")
    print(f"name: {form.name}")
    # for arg in sys.argv:
    #     print(f"arg = {arg}")
    # print(f"arguments: {sys.argv}\n")
    # print("arguments:\n" + "\n".join(sys.argv))
    # print(os.environ)

do_my_stuff()
