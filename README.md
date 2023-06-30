# ocen

`ocen` aims to provide you the same level of control over your code as C, but with some modern features like
type inference, namespaces, stricter types and a module system. It transpiles to C, and can easily bind to C libraries.

## Usage

Use the following command to build the initial compiler:

```shell
$ ./meta/bootstrap.sh       # Generates ./bootstrap/ocen if successful
```

### Compiling other programs

For now, the compiler _needs_ to be run from the root of the repository, since it needs to be able 
to find the `std` directory containing the standard library.

```shell
$ ocen file.oc                # generates ./out and ./out.c
$ ocen file.oc -o ./build/out # generates ./build/out and ./build/out.c
$ ocen file.oc -n -c ./temp.c # generates only ./temp.c
```

For tips on developing the compiler, see [DEVELOPING.md](./DEVELOPING.md).

## Language

You can find various examples of the language in:
- [tests](./tests): Unit tests for the compiler
- [std](./std): The standard library (implementations + C bindings)
- [examples](./examples): Various examples of the language
- [compiler](./compiler): The compiler itself!