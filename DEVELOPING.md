## Running unit tests on the compiler:

The `meta/test.py` script runs the unit tests on the compiler. It requires the path to the compiler
executable to be provided. It also requires `python3` to be installed.

```shell
$ ocen compiler/main.oc -o build/ocen
$ ./meta/test.py -c ./build/ocen

# or a one liner:
$ ocen compiler/main.oc -o build/ocen && ./meta/test.py -c ./build/ocen
```

## Debugging using `gdb`/`lldb`

There's some basic support for debugging the compiler / programs using `gdb` or `lldb`. To get an executable with
debug symbols, use the `-d` flag when compiling. This can then be used with a debugger.

```shell
$ ocen -d program.oc -o build/prog
$ gdb build/prog
```

Note: GDB doesn't know about `ocen` syntax, so if you want to evaluate expressions / call methods on objects, you'll
need to manually call the correct functions and pass in the arguments. For details on the actual translation from `ocen`
to `C`, see [codegen](./compiler/passes/code_generator.oc).

## Development Workflow

If you wish to develop on the compiler, the recommended flow for development is:

```bash
$ ./meta/bootstrap.sh # Generates the initial compiler
$ touch tests/feature.oc # Create a test your feature

# Now, edit the compiler to add the feature you want to add
# Build the new compiler, and verify it works with the test
$ ./bootstrap/ocen compiler/main.oc -o build/ocen && ./build/ocen tests/feature.oc

$ ./meta/test.py -c ./build/ocen # Make sure all tests pass
$ ./meta/gen_bootstrap.sh # Generate the new bootstrap compiler
```

Essentially, keep `bootstrap/ocen` as the canonical compiler which will not break, and `build/ocen`as the temporary build for testing.

Once significant language features are implemented, we should update the bootstrapped files before
we actually use any of these features in the compiler itself, to avoid any circular issues. For instance: if add
feature XYZ to the language and immediately use feature XYZ in the compiler (without updating the bootstrap) it will not work.
