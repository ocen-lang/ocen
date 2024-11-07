# Style Guide

There is no formatter for Ocen (yet), so please make sure any code you submit for a PR
adheres to the style conventions used through the codebase. Any PRs not adhering to the
style guide will not be merged until all the recommendations here are followed.

## Naming

### Cases to use

#### `PascalCase`

- Structs
- Unions
- Enums
- Enum Values
- TypeDefs

#### `snake_case`

- Variables (Global and local)
- Parameters
- Struct/Union fields
- Files / Folders
- Explicitly created namespaces (with `namespace` keyword)

#### `UPPER_SNAKE_CASE`

- Constants (only allowed in global scope)

### Naming Conventions

#### Functions / Methods

- Getters **should not** use `get_` prefix. Avoid trivial getters for fields that already exist.

- Setters **should** use `set_` prefix. Avoid trivial setters for fields that already exist, unless extra functionality is needed.

- Functions/Methods that check some property (and return a boolean) **should** use `is_` prefix / 
  somehow indicate that the function returns a boolean from the name
    - Good: `is_file(...)`, `dir_exists(...)`, `should_send_request()`
    - Bad: `get_file(...)`, `find_dir(...)`

- Functions/Methods that perform some action should use a verb.
    - Good: `Foo::check_bar()`, `Foo::send_request()`
    - Bad: `Foo::bar_checker()`, `Foo::requesting()`

- Constructor functions that return a pointer to allocated memory should be called `new`.
    - Good: `Foo::new(...): &Foo`, `Bar::new(...): &Bar`
    - Bad: `Foo::create(...)`, `Bar::make_new(...)`

- Constructor functions that return a value should be called `make`.
    - Good: `Foo::make(...): Foo`, `Bar::make(...): Bar`
    - Bad: `Foo::new(...)`, `Bar::make_new(...)`

- Destructor functions should be called `free`.



#### Variables

- Don't try to keep variable / field names short at the cost of readability. A good variable name should
  make clear what it's purpose is without needing extra comments/documentation.

#### (Explicit) Namespaces

- Use `impl::` as a namespace for internal functions in a file. Avoid having separate `impl::` blocks in most cases, and use a single one.
- Should always be in `snake_case`

## Formatting

### Spacing

> [!NOTE]
> No trailing spaces. Ever. All of the rules below assume this.

- Indents are 4 spaces. No tabs.

- Always have a space on **both sides** for:
    - `=` (assignment)
    - `+`,`-`,`*`,`/`,`%`, ... (all binary operators)
    - `|` when used as a separator in a match statement
    - `=>` in arrow functions / match statements
    - `in` for for loops / `in` operator
    - `{` and `}` when used in blocks: `if foo { ... } else if bar { ... }`, `def foo() { ... }`

- Always **have** a space **after** (if something else on the same line):
    - `,` for lists, parameters, templates, etc: `foo(1, 2, 3)`, `let foo = [1, 2, 3]`
    - `:` for type annotations, etc: `foo: Bar`, `def foo(): Bar`
    - `;` in for loops, array type annotations, etc: `for let x = 0; x < 10; x++ {`, `[i32; 10]`
    - `//` for comments (if comment is non-empty)
    - `{` when used in imports: `import std::foo::{ bar, baz }`  (and space before `}`)


- **Don't** have spaces for:
    - Unary operators: `~foo`, `-bar`, `ptr?`, etc.
    - `.` for this fields/methods: `.this`, `.that()`
    - `[]` for lists, indices, etc: `list[0]`, `[1, 2, 3]`
    - `()` for function calls: `foo()`, `bar(1, 2, 3)`
    - `<>` for templates: `Foo<Bar>`, `List<i32, Bar>`
    - `&` for address / type annotations: `&foo`, `&mbar`
    - Between `fn` and `(` for function type annotations: `fn(i32): i32`


- For everything else, use common sense. If it looks better with a space, add a space.
  If not specified here, look at the codebase for examples. If no examples are found, ask.

- It's OK to add extra spaces **within** a line to align code, but don't go overboard. If aligning, make sure every
  line at least has the minimum spaces to meet the above rules. You should never have extra spaces (outside of
  the current indentation level) at the start of a line.
    - Use an anonymous block to logically group code if you needed.

### Misc

- Don't use semicolons, except when syntactically required (for loops, array types)

- Prefer to use `{}` for blocks, even if they are one-liners. This makes it easier to add more code later.

- If a function is a one-liner, it should be an arrow-function. If it's not, it should be a block.

- Don't add extra parentheses unless they are needed for precedence.
    - Good: `if a < b < c { ... }`, `while foo { ... }`
    - Bad: `if (a < b) < c { ... }`, `while (foo) { ... }`

- Use `then` for one-line if statements. This makes it clear that the next line is the body of the if statement.
    - Good: `if foo then bar()`
    - Bad: `if foo { bar() }`

## Code Organization

### Imports / Modules

- Avoid using `import *` in most cases - you should never do this on your own unless explicitly suggested.

- Avoid importing utility functions from the (immediate) `std::` namespace, instead use them with the `std::` prefix. (This does not apply to any of the sub-modules of `std::`).
    - Good: `std::exit(...)`, `std::panic(...)`
    - Bad:  `import std::{exit,panic} ... exit(...) ... panic()`

- All non-module imports should be in `{ ... }`, even if not required:
    - Good: `import std::vector::{ Vector }`
    - Bad:  `import std::vector::Vector`

- Try to avoid functions to the scope whenever possible. Use them with the module prefix. We make an exception for types, as they are used more frequently when annotating variables.
    - Good: `import std::libc ... libc::strerror(...)`
    - Bad: `import std::libc::{ stderror }  ... strerror(...)`

- Don't nest multi-imports more than 2 levels deep. If you need to import more than 2 levels deep, use multiple import statements.
    - Good: `import std::foo::{ f0, f1::{ f11 } }; import std::bar::{ qux }`
    - Bad:  `import std::{ foo::{ f0, f1::{ f11 } }, bar::{ qux } }`

- Use package-relative imports `import @foo::` instead of relative `import ..foo::` imports. This makes it easier to move files around without breaking imports. Exception is for `mod.oc` files, which can use relative imports in the current directory.

### Binding C Functions

- It's OK to expose C structs/functions directly if it makes sense. If the API is clunky, wrap it in a more idiomatic way.

- Always use `[extern]` to bind functions/structs/etc. The old postfix `extern` syntax will be deprecated.
    - Good: `[extern "foo_t"] struct Foo{}`
    - Bad:  `struct Foo extern("foo_t") {}`

- All bindings should follow Ocen naming conventions. Rename the bound function if necessary. This should
  even apply if the binding is internal-only in an `impl::` namespace, unless this causes unnecessary confusion.
    - Good: `[extern "foo_t] struct Foo{}`
    - Bad: `[extern] struct foo_t{}`

- Whenever possible, method-like functions should be bound as a method on the parent object.
    - Good: `[extern] struct Foo{}; [extern "foo_bar"] def Foo::bar(this)`
    - Bad:  `[extern] struct Foo{}; [extern] def foo_bar(foo: Foo)`

- When binding a bunch of constants intended to behave like an enum, bind them directly to an enum instead.
    - [Example on how to do this](https://github.com/ocen-lang/ocen/blob/master/GETTING_STARTED.md#miscellaneous-binding-tips)


### Misc

- All calls to variadic-format functions (including `print` and `println`) should use format-strings instead of manually specifying the format. Exceptions are when you have a complicated format string that can't be easily expressed with the format macro, and you don't want to add a `formatting` attribute to the struct.
    - Good: `print(f"sum = {1+2}")`
    - Bad:  `print("sum = %d", 1+2)`

- In for loops, prefer to use `i++` instead of `i += 1`