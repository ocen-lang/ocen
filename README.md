# Aecor 2.0

Mildly WIP. Actually kind-of usable. Need to have original `aecor` environment setup, [look here](https://github.com/mustafaquraish/aecor).

## Usage

Build and run the compiler:

```shell
$ aecor compiler/main.ae -o aecor2
$ ./aecor2 --help
$ ./aecor2 test/test.ae
```

Convenient flow when working on a compiler feature:
```shell
# create some sample code with the feature in `feature.ae`
# Use this to re-build compiler and run it on the sample code
$ aecor compiler/main.ae -o aecor2 && ./aecor2 feature.ae
```

Running unit tests on the compiler:
```shell
$ aecor compiler/main.ae -o aecor2
$ ./meta/test.py -c ./aecor2 # pass path to the built compiler (NEED THE ./)

# or a one liner:
$ aecor compiler/main.ae -o aecor2 && ./meta/test.py -c ./aecor2 # (NEED THE ./)
```

*Note*: The unit tests are imported from aecor, and most do not pass yet because of different
error messages and/or unimplemented features.