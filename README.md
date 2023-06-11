# Aecor 2.0

Heavily WIP. Need to have original `aecor` environment setup, [look here](https://github.com/mustafaquraish/aecor).

## Usage

Build and run the compiler:

```shell
$ aecor compiler/main.ae -o compiler
$ ./compiler --help
$ ./compiler test/test.ae
```

Convenient flow when working on a compiler feature:
```shell
# create some sample code with the feature in `feature.ae`
# Use this to re-build compiler and run it on the sample code
$ aecor compiler/main.ae -o compiler && ./compiler feature.ae
```

Running unit tests on the compiler:
```shell
$ aecor compiler/main.ae -o compiler
$ ./meta/test.py -c compiler # pass path to the built compiler

# or a one liner:
$ aecor compiler/main.ae -o compiler && ./meta/test.py -c compiler
```

*Note*: The unit tests are imported from aecor, and most do not pass yet because of different
error messages and/or unimplemented features.