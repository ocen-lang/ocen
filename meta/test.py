#!/usr/bin/env python3
from collections import namedtuple
import shutil
from subprocess import run, PIPE
import argparse
import re
from ast import literal_eval
from dataclasses import dataclass
from enum import Enum
from os import system, makedirs
from pathlib import Path
from sys import argv
from typing import Union, Optional, Tuple
import multiprocessing
import sys
import shlex
import json
import textwrap

class Result(Enum):
    EXIT_WITH_CODE = 1
    EXIT_WITH_OUTPUT = 2
    COMPILE_FAIL = 3
    COMPILE_SUCCESS = 4
    SKIP_SILENTLY = 5
    SKIP_REPORT = 6
    RUNTIME_FAIL = 7
    LSP = 8

@dataclass(frozen=True)
class LSPTest:
    flags: str
    value: str

@dataclass(frozen=True)
class Expected:
    type: Result
    value: Union[int, str, None]


def get_expected(filename) -> Optional[Expected]:
    with open(filename, encoding="utf8", errors='ignore') as file:
        is_lsp = False
        lsp_flags = ""

        for line in file:
            if not line.startswith("///"):
                break

            line = line[3:].strip()

            # Commands with no arguments
            if line == "skip":
                return Expected(Result.SKIP_SILENTLY, None)
            if line == "compile":
                return Expected(Result.COMPILE_SUCCESS, None)
            if line == "":
                continue

            if ":" not in line:
                print(f'[-] Invalid parameters in {filename}: "{line}"')
                break

            # Commands with arguments
            name, value = map(str.strip, line.split(":", 1))

            if name == "exit":
                return Expected(Result.EXIT_WITH_CODE, int(value))
            if name == "out":
                return Expected(Result.EXIT_WITH_OUTPUT, value)
            if name == "fail":
                return Expected(Result.COMPILE_FAIL, value)
            if name == "runfail":
                return Expected(Result.RUNTIME_FAIL, value)
            if name == "lsp":
                is_lsp = True
                lsp_flags = value
                break

            print(f'[-] Invalid parameter in {filename}: {line}')
            break

        if is_lsp:
            try:
                value = next(file)
                if not value.startswith("/// "):
                    print(f'[-] Expected JSON in {filename} after LSP directive')
                    return Expected(Result.SKIP_REPORT, None)
                data = json.loads(value[4:])
                return Expected(Result.LSP, LSPTest(lsp_flags, data))
            except StopIteration:
                print(f'[-] Expected JSON in {filename} after LSP directive')
                return Expected(Result.SKIP_REPORT, None)
            except json.JSONDecodeError:
                print(f'[-] Failed to parse JSON in {filename} after LSP directive')
                return Expected(Result.SKIP_REPORT, None)


    return Expected(Result.SKIP_REPORT, None)

def check_lsp_output(smol, big):
    if not type(smol) == type(big):
        return False
    if isinstance(smol, (int, float, str)):
        return smol == big

    if isinstance(smol, list):
        for i, val in enumerate(smol):
            if not any(check_lsp_output(val, big_val) for big_val in big):
                return False
        return True

    if isinstance(smol, dict):
        for key, value in smol.items():
            if key not in big:
                return False

            # For file key, we want assume the smol value is a regex
            if key == "file":
                if not re.match(value, big[key]):
                    return False

            if not check_lsp_output(value, big[key]):
                return False
        return True
    return smol == big

def handle_lsp_test(compiler: str, num: int, path: Path, expected: Expected, debug: bool) -> Tuple[bool, str, Path]:
    cmd = f'{compiler} lsp {expected.value.flags} {path}'
    if debug:
        print(f"[{num}] {path} || {cmd}", flush=True)
    process = run(shlex.split(cmd), stdout=PIPE, stderr=PIPE)
    if process.returncode != 0:
        return False, f"LSP Command failed with code: {process.returncode}, stdout: {process.stdout}", path

    output = process.stdout.decode('utf-8').strip()
    try:
        output = json.loads(output)
    except json.JSONDecodeError:
        return False, f"Failed to parse JSON output: {output}", path

    wanted = expected.value.value
    if check_lsp_output(wanted, output):
        return True, "(Success)", path

    return False, f"Expected LSP output does not match\n  expected: {wanted}\n       got: {output}", path

def handle_test(compiler: str, num: int, path: Path, expected: Expected, debug: bool) -> Tuple[bool, str, Path]:
    exec_name = f'./build/tests/{path.stem}-{num}'
    if debug:
        print(f"[{num}] {path} || {exec_name}", flush=True)

    if expected.type == Result.LSP:
        return handle_lsp_test(compiler, num, path, expected, debug)

    process = run(
        [compiler, str(path), '-o', exec_name],
        stdout=PIPE,
        stderr=PIPE
    )
    if expected.type == Result.COMPILE_FAIL:
        if process.returncode == 0:
            return False, "Expected compilation failure, but succeeded", path

        error = process.stdout.decode("utf-8").strip()
        error += process.stderr.decode("utf-8").strip()
        expected_error = expected.value

        if expected_error in error:
            return True, "(Success)", path
        else:
            try:
                remaining = error.split("Error: ")[1]
            except IndexError:
                remaining = error
            return False, f"Did not find expected error message\n  expected: {expected_error}\n       got: '{remaining}'", path

    elif process.returncode != 0:
        stdout = textwrap.indent(process.stdout.decode("utf-8"), " "*10).strip()
        stderr = textwrap.indent(process.stderr.decode("utf-8"), " "*10).strip()
        return False, f"Compilation failed:\n  code: {process.returncode}\n  stdout: {stdout}\n  stderr: {stderr}", path

    elif expected.type == Result.COMPILE_SUCCESS:
        return True, "(Success)", path

    try:
        process = run([exec_name], stdout=PIPE, stderr=PIPE)
    except FileNotFoundError:
        return False, "Executable not found", path

    if process.returncode != 0 and expected.type not in [Result.EXIT_WITH_CODE, Result.RUNTIME_FAIL]:
        return False, f"Expected exit code 0, but got {process.returncode}", path

    if expected.type == Result.EXIT_WITH_CODE:
        if process.returncode != expected.value:
            return False, f"Expected exit code {expected.value}, but got {process.returncode}", path

    if expected.type == Result.EXIT_WITH_OUTPUT:
        stdout_output = process.stdout.decode('utf-8').strip()
        stderr_output = process.stderr.decode('utf-8').strip()
        output = (stdout_output + '\n' + stderr_output).strip()
        expected_out = literal_eval(expected.value).strip()
        if output != expected_out:
            return False, f'Incorrect output produced\n  expected: {repr(expected_out)}\n       got: {repr(output)}', path

    if expected.type == Result.RUNTIME_FAIL:
        output = process.stdout.decode('utf-8').strip()
        output_err = process.stderr.decode('utf-8').strip()
        expected_out = expected.value
        if expected_out not in output + output_err:
            return False, f'Expected runtime error not found\n  expected: {repr(expected_out)}\n  got: {repr(output)}', path

    return True, "(Success)", path

def pool_helper(args):
    return handle_test(*args)

def output_test_results(result, stats, debug):
    passed, message, path = result
    if sys.stdout.isatty() and not debug:
        print(f" \33[2K[\033[92m{stats.passed:3d}\033[0m", end="")
        print(f"/\033[91m{stats.failed:3d}\033[0m]", end="")
        print(f" Running tests, finished {stats.passed+stats.failed} / {stats.total}\r", end="", flush=True)
    if passed:
        stats.passed += 1
    else:
        stats.failed += 1
        if sys.stdout.isatty() and not debug:
            print(f"\33[2K\033[91m[-] Failed {path}\033[0m")
            print(f"  - {message}", flush=True)
        else:
            print(f"[-] Failed {path}")
    if debug and not passed:
        exit(1)


def main():
    parser = argparse.ArgumentParser(description="Runs ocen test suite")
    parser.add_argument(
        "-c",
        "--compiler",
        default="ocen",
        help="Path to the compiler executable"
    )
    parser.add_argument(
        "-d",
        "--debug",
        action="store_true",
        help="Run single threaded, and print all test names as we go"
    )
    parser.add_argument(
        "files",
        nargs="?",
        default=["tests"],
        help="Files / folders to run"
    )
    args = parser.parse_args()
    arg_files = args.files if isinstance(args.files, list) else [args.files]
    test_paths = [Path(pth) for pth in arg_files]

    tests_to_run = []
    for path in test_paths:
        files = []

        if path.is_dir():
            for path_ in path.glob('**/*'):
                if path_.is_file():
                    files.append(path_)
        else:
            files.append(path)

        for file in files:
            expected = get_expected(file)
            if expected.type == Result.SKIP_SILENTLY:
                continue
            if expected.type == Result.SKIP_REPORT:
                print(f'[-] Skipping {file}')
                continue
            tests_to_run.append((file, expected))

    tests_to_run.sort()

    # Dummy empty class to store stats, and pass mutable object to pool_helper
    class Stats(): pass
    stats = Stats()
    stats.passed = 0
    stats.failed = 0
    stats.total = len(tests_to_run)

    arguments = [
        (args.compiler, num, test_path, expected, args.debug)
        for num, (test_path, expected) in enumerate(tests_to_run)
    ]

    # Clear out existing test directories
    shutil.rmtree("build/tests", ignore_errors=True)
    makedirs("build/tests", exist_ok=True)

    if args.debug:
        for inp in arguments:
            result = pool_helper(inp)
            output_test_results(result, stats, True)

    else:
        with multiprocessing.Pool(multiprocessing.cpu_count()) as pool:
            for result in pool.imap_unordered(pool_helper, arguments):
                output_test_results(result, stats, False)

    if sys.stdout.isatty():
        print("\33[2K")
        print(f"Tests passed: \033[92m{stats.passed}\033[0m")
        print(f"Tests failed: \033[91m{stats.failed}\033[0m")

    if stats.failed > 0:
        exit(1)


if __name__ == "__main__":
    main()
