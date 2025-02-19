#!/usr/bin/env python3

import shutil
import re
import subprocess as sp
from sys import argv
import json
from pathlib import Path
import clipboard

_, inp_file, args, out_file = argv

# Rebuild compiler...
sp.run("ocen compiler/main.oc -o ./build/ocen", shell=True, check=True)
lsp_cmd = f"./build/ocen lsp {args} {inp_file}"
expected = sp.check_output(lsp_cmd, text=True, shell=True)
try:
    expected = json.loads(expected)
except json.JSONDecodeError:
    print(f'[ERROR]: Failed to parse JSON output: `{expected}`')
    print()
    cmd = "ocen compiler/main.oc -o ./build/ocen && " + lsp_cmd
    print('Command:')
    print(cmd)
    clipboard.copy(cmd)
    exit(1)



res = re.search(r"([-\w]+) (\d+) (\d+)", args)
if res:
    # we have line and column numbers. We want to add 3 to the line number
    # to account for the 3 lines of metadata we are adding
    cmd, line, column = res.groups()
    line = int(line) + 3
    args = f"{cmd} {line} {column}"


def normalize_paths(data):
    if isinstance(data, dict):
        for key, value in data.items():
            if key == "file":
                data[key] = Path(value).basename()
            else:
                normalize_paths(value)
    elif isinstance(data, list):
        for value in data:
            normalize_paths(value)
normalize_paths(expected)

with open(inp_file, "r") as f:
    inp = f.read()

out = f"""\
/// lsp: {args}
/// {json.dumps(expected)}

{inp}
"""

print(out)

with open(out_file, "w") as f:
    f.write(out)

