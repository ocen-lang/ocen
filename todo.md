
- [ ] Implement `assert` with proper locations / error messages
    - [ ] Also maybe static_asserts?

- [ ] Templates!
    - [ ] Templated functions
    - [ ] Templated structs
        - [ ] Templated struct methods

- [ ] Macro system
    - [ ] AST based macros? What does the syntax look like?

- [ ] Compile time if/other logic
    - [ ] Including different files based on compile time logic?

- [ ] Better support for hiding / exposing symbols in namespaces
    - [ ] `public` and/or `private` keywords? Which one is default?
    - [ ] `export` keyword for exporting symbols from a module?
        - [ ] `reexport` keyword for reexporting symbols from a module? Or same as above?

- [ ] Support for C-like variadics, so we can have `printf` and friends without compiler hacks
    - [ ] How does this work with templates? Any chance of figuring out variadic templates?

- [ ] Advanced inference for variables based on first usage. For instance, `let x = 0; x < 5i32;` should make `x` be an `i32`

- [ ] Sized Array literals

- [ ] Allow using `-` or something similar in struct constructors to skip initializing fields

- [ ] `new` keyword (or something similar) for allocating objects on the heap so we don't have to call `calloc` manually
    -  For instance, `let x = new Foo(a: 4, b: ---)`