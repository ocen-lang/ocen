# Ocen 2.0

Mildly WIP. Actually kind-of usable. Need to have original `ocen` environment setup, [look here](https://github.com/mustafaquraish/ocen).

## Usage

Build and run the compiler:

```shell
$ ocen compiler/main.oc -o ocen2
$ ./ocen2 --help
$ ./ocen2 test/test.oc
```

Convenient flow when working on a compiler feature:
```shell
# create some sample code with the feature in `feature.oc`
# Use this to re-build compiler and run it on the sample code
$ ocen compiler/main.oc -o ocen2 && ./ocen2 feature.oc
```

Running unit tests on the compiler:
```shell
$ ocen compiler/main.oc -o ocen2
$ ./meta/test.py -c ./ocen2 # pass path to the built compiler (NEED THE ./)

# or a one liner:
$ ocen compiler/main.oc -o ocen2 && ./meta/test.py -c ./ocen2 # (NEED THE ./)
```

*Note*: The unit tests are imported from ocen, and most do not pass yet because of different
error messages and/or unimplemented features.