# ocen

`ocen` is intended for people who enjoy the do-it-yourself nature of C, and the level of control it gives you - but don't want to deal with the syntactical nuances of C. `ocen` fundamentally behaves and programs very much like C, but offers some quality-of-life features such as:

- Proper namespaces, modules and import system (say goodbye to header files, and order-dependent definitions)
- Stronger types (almost no implicit casting), and type inference for variables
- Ability to attach methods to structs, enums, builtins
- Rich standard library, with maps, lists, binary IO, image IO, graphics bindings (SDL, GLUT), etc
- Format strings, with JS (`` `hello {name}` ``) and Python (`f"val = {x:0.3f}"`) syntaxes supported (but they allocate)
- Better enums: all values are namespaced, can be automatically debug-printed in format strings
- Ability to easily bind C libraries, and to be able to use external functions as methods
- Syntactic sugar for `for-each` loops, as long as the object has necessary iterator methods
- Automatically generate `json` for documentation with all definitions in code, and extracted comments
- Relatively small and hackable code-base

`ocen` transpiles to C, and the _only_ dependency for this compiler is a C compiler on your system (`gcc` by default, but can be specified with `CC` environment variable). For now `ocen` is only tested/supported on linux and macOS, but contributions to help get this running on Windows are welcome if anyone is interested.

## Installing

### Automated Installation

For the _"I don't care, just give me the command"_ people. **This will add some environment variables to your shell config**

```shell
# Default (installs in ~/ocen):
$ curl -sSL https://raw.githubusercontent.com/ocen-lang/ocen/master/meta/install.sh | bash

# Custom install path:
$ curl -sSL https://raw.githubusercontent.com/ocen-lang/ocen/master/meta/install.sh | BASE=~/local/ bash
```

### Manual Installation

Clone the repository:

```shell
$ git clone https://github.com/ocen-lang/ocen.git
$ cd ocen
```

Use the following command to build the initial compiler:

```shell
$ ./meta/bootstrap.sh  # Generates initial compiler
$ ./bootstrap/ocen --help
```

### Compiling other programs

If you wish to use `ocen` from elsewhere, you can set the `OCEN_ROOT` environment variable to point
to the root directory of this project. This lets the compiler find the standard library and other
files it needs to compile your program.

```shell
$ export OCEN_ROOT=/path/to/ocen
$ export PATH=$OCEN_ROOT/bootstrap:$PATH # Add the compiler to your PATH
```

You can then use the compiler as follows:

```shell
$ ocen file.oc                # generates ./out and ./out.c
$ ocen file.oc -o ./build/out # generates ./build/out and ./build/out.c
$ ocen file.oc -n -c ./temp.c # generates only ./temp.c
```

## Usage

- For an overview of the language and it's features, look at the [getting started doc](./docs/GETTING_STARTED.md).
- For tips / workflow on developing the compiler, look at the [compiler development doc](./docs/DEVELOPMENT.md).
- For recommended code style (used in the repo), look at the [style guide](./docs/CODE_STYLE.md).


You also can find various examples of the language being used in:
- [tests](./tests): Unit tests for the compiler (shows all the features)
- [std](./std): The standard library (implementations + C bindings)
- [examples](./examples): Few non-trivial examples of programs
- [compiler](./compiler): The compiler itself!

