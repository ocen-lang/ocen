# Installation

> [!NOTE]
> Ocen is tested on Linux and macOS. Windows (and MSVC) are not supported
> at the moment. You can use WSL on Windows to use Ocen, or possibly try
> your luck with MinGW / etc. The standard library heavily relies on
> Unix-like C functions and system calls.

Ocen requires a C compiler to be available on the system. It uses `gcc`
available on your `PATH` to compile the resulting C code. Alternatively,
you can use the `CC` environment variable to specify a different
compiler.

## Building from source

``` bash
$ git clone https://github.com/ocen-lang/ocen ~/ocen
$ cd ~/ocen
$ ./meta/bootstrap.sh
$ ./bootstrap/ocen --help
```

## Environment Setup

In order to use Ocen to compile and run other programs, you\'ll need to
set up your environment so the compiler can find the standard library
and other necessary files. Add these lines to your shell profile (e.g.
`~/.bashrc` or `~/.zshrc`):

``` bash
$ export OCEN_ROOT=$HOME/ocen             # For standard library and other files
$ export PATH=$OCEN_ROOT/bootstrap:$PATH  # Add the compiler to your PATH
```

You should now be able to invoke the Ocen compiler from anywhere on your
system:

``` bash
$ ocen --help
```

## VSCode Extension

Ocen has a VSCode extension that provides syntax highlighting and some
basic LSP features. For the extension to work, you need to have the Ocen
compiler available on your `PATH`. The extension can be found on the
[VSCode
Marketplace](https://marketplace.visualstudio.com/items?itemName=MustafaQuraish.ocen).

> [!NOTE]
> The extension is still in development and you are likely to encounter
> bugs. Please report any that you find. It currently works on a
> per-document basis, so any changes made in other open files will not
> automatically be reflected in other open files.

Some tips for using the extension:

-   The extension provides an `Ocen: Rescan document` command to re-run
    the LSP server on the current document. This is useful if you\'ve
    modified another file and want to update the current document with
    the changes.
-   The extension provides some convenient snippets for common
    constructs in Ocen.

# Language

## Annotated Hello World

```rust
// Comments

// Entry point is `main`. Arguments for main are optional, and
// Return type is implicitly `i32`
def main(argc: i32, argv: &str) {
    // `print` and `println` are builtin functions, that work like C
    println("Hello world from %s", "ocen")
}

```

## Builtins

The builtin types in Ocen include:

- `u8`, `i8`, `u16`, `i16`, `u32`, `i32`, `u64`, `i64`: Signed integer types
- `f32`, `f64`: Floating point types
- `char`: Character type - one byte. (Equivalent to `char` in C)
- `str`: Similar to `char*` in C, and can be used interchangebly with `&char`
- `untyped_ptr`: Similar to `void*` in C. It can be implicitly cast to/from any other pointer.
- `bool`: A boolean type


### Type Modifiers

All types can be modified with some builtin operations. Using `u32` as an example:

- `&u32`, `&&u32`: A pointer to `u32`, and a pointer to a pointer to `u32`
- `[u32; 5]`: An array of 5 `u32`s.
- `fn(u32, u32): u32`: A function-type that takes in 2 `u32`s and returns a `u32`


## Strings

### String Literals

String literals are all C-style. They are null-terminated, and stored as global
constants

```rust
let s = "Hello, I'm a constant string"
```

### Format Strings

Additionally, There are format strings available, using `\`` or Python-style `f""` (no difference,
just preference). Unless they are directly being passed to a variadic function, they will
dynamically allocate memory, which needs to be freed by the user. They are null-terminated.

```js
// NOTE: no `$` is needed before the curly brackets.
let a = `X is {X} and 3+4 = {3 + 4}`
let b = `I allocate memory. {{`        // Use double curly to escape it
let c = f"0.1 + 0.2 = {0.1+0.2:.1f}"   // Explicit format specifier

// `print` and `println` functions are variadic - no allocation happens here,
// and this expands to format specifiers + arguments in generated C.
println(`Some math {1+2+3}`)
```


## Variables, and literals

Variables are declared with the `let` keyword, and they are lexically scoped. Shadowing is
permitted, except in the same scope. Unlike in C, annotating a type is optional.

```rust
let a = 5       // Literal integers are u32 by default
let b = -6      // Negative literals are i32 by default
let c: u8 = 7   // Adding a type annotation makes literals the correct type
let d = 7u8     // Literal integers can have an integer-type suffix.
let e = "hi"    // str type
let f = 3.14    // Implicitly an f32, but can use annotations

let x: str      // Uninitialized variable (MUST provide a type)
```

## Global variables and constants

Ocen supports global variables and constants. Global variables are similar to local
ones, with the exception that type annotations **need** to be specified, and you can only use
literals / other globals to initialize them, not arbitrary expressions such as function calls.

```rust
let g_x: i32 = 0          // OK!
let g_z: i32 = g_x + 1    // OK!
let g_y = g_x             // ERROR: No type-annotation.
let g_w: i32 = foo()      // ERROR: Function call not allowed
let g_a: i32              // OK! Zero-initialized by default.

def main() => 0
```

Constants are defined with `const`, and are **only** allowed in the global scope. They are
compile-time constant values, and have similar restrictions as globals. Constants **must** be
initialized when they are defined.

> [!NOTE]
> Constants compile down to `#define ...` at the C level. This is so you can use them to define
> the size of arrays / etc. which is not not possible with global variables. However this does
> limit what you can do with them, and many of the restrictions are because of this.

```rust
let g_A: i32 = 0

const X: i32 = 0        // OK!
const Y: i32 = X * 3    // OK! Can use other constants
const Z: i32 = g_A * 2  // ERROR: Can't use variable in constant expression

def main() => 0
```


## Arithmetic, Comparisions, other operators

All numerical operations in Ocen are strictly typed, and both operands must have the same type.

```rust
let x = 5 + 10          // OK: Implicitly both u32
let y = 5u64 + 10       // OK: `10` here picks up the type from the LHS
let z = 7u8 + 9i8       // ERROR: Both LHS and RHS have different types.
```

All available operators:
```cpp
// Arithmetic
a + b    a += b
a - b    a -= b
a * b    a *= b
a / b    a /= b
a % b
-a


// Logical
a and b
a or b
not a

// Comparison
a > b   a >= b
a < b   a <= b
a == b  a != b
a < b <= c < d       // Allowed

// Increment / Decrement
++x
x++
--x
x--

// Bitwise operators
~x      // Complement
x ^ y   // XOR
x | y   // OR
x & y   // AND

// Bitshift operators
x << y      x >>= y
x >> y      x <<= y

// Misc:
x?      // Question mark checks if pointer is non-null. Only valid for pointer types.
x in y  // No default implementations, but can override
```

> [!WARNING]
> Make sure you don't use expensive expressions (or with side-effects) in multiple-comparisons.
> They will be evaluated multiple times. It's recommended to only use these with variables.


### Pointer Arithmetic

Pointer arithmetic is treated specially. You can add/subtract any pointer type `P` with any
integer-like type `I` to get back a value with type `P`

```rust
let x = "Hello" + 2     // OK: A pointer to the the first `l` in the string
let y: str = x - 2i8    // OK: Get back a pointer to the `H`
```

## Arrays, Pointers, and Indexing

Arrays always decay to a pointer when referred to / passed around. Indexing into an array/pointer
is supported with any integer type.

```rust
let x = [1, 2, 3]           // x: [u32; 3]
let y: [str; 5]             // Zero-intialized by default
let z: &u32 = x             // `x` decays to a pointer
let w = z[2i8]              // Indexing OK with any integer type
```

## Control flow

### If statements

Ocen has 2 types of `if` statements: the regular kind, and what is internally referred to as
`multi-if`. They are semantically equivalent, and can be used based on syntactical preference.
No parenthesis are needed around the conditions.

```rust
// Regular if statement
if foo {
   do_foo()
} else if bar {
   do_bar1()
   do_bar2()
} else {
   do_baz()
}

// Equivalent Multi-if
if {
   foo => do_foo()
   bar => {
      do_bar1()
      do_bar2()
   }
   else => do_baz()
}
```

## While Loops

Pretty much what you expect.

```rust
while a < 5 {
   do_something()
}
```

## For loops

Standard C-style for loops are available. None of the blocks can be empty (for now). Recommended
to use a placeholder in case this is needed.

```rust
for let i = 0; i < 10; i++ {
   do_something()
}
```

### For-each loops

On supported iterator types, you can use the following syntax. More details on this are in
[[TODO: Iterator methods]]

```rust
for x in list.iter() {
   do_something(x)
}
```


## Casting

You can cast values between types using the `as` keyword.

> [!WARNING]
> Casting is *not* checked by the Ocen compiler - it assumes that it's valid.
> This can be useful for bypassing certain quirks when interfacing with C types,
> but can also lead to breaking code. Be careful when doing this. It's possible
> to get invalid C code from this that may not compile.

```rust
let x = 5u8 as u32      // OK
let y = -1i64 as u8     // OK: We don't check for underflow/overflow
let z = "hi" as u8      // OK(!!): This makes no sense, but we don't check.
```

## Functions

Functions are defined with `def`. All parameters must be typed. A return type is optional. If a
function has a return type, it must return a value explicitly.

No function declarations are needed, and you are allowed to use functions declared
later in the file.

```rust
def foo(a: u32, b: u32): u32 {
   let c = a + b
   return c
}
```

### Parameter labels

When calling a function, the parameters **always** need to be passed in the same order they are
specified. You can optionally specify the name of the parameter when calling it, to make the
intention clearer at the call site. If an incorrect label is used, this will trigger an error.

```rust
def verify(check_a: bool, check_b: bool, check_c: bool): bool => ...

// You can do this
verify(true, false, true)

// But clearer to do this
verify(check_a: true, check_b: false, check_c: true)

// Can mix and match, depending on preference
verify(true, check_b: false, true)
```

### Default arguments

Ocen allows having default arguments for functions. All the default arguments need to come at the
end of the parameter list.

```rust
def foo(a: u32, b: u32 = 10): u32 => a + b

let x = foo(1, 2)
let y = foo(3)
```

> [!NOTE]
> It is not possible to provide a value for a default argument `B` that comes after a default
argument `A` without also providing a value for `A`. This may be fixed in the future.

```rust
def bar(a: u32, b: u32 = 0, c: u32 = 1): u32 => a + b + c

let x = bar(1, 2, 3)    // OK!
let y = bar(1)          // OK!
let z = bar(1, b: 2)    // OK!
let w = bar(1, c: 2)    // ERROR: Need to provide `b` if you want to provide `c`
```


### Arrow functions

If a function returns a single expression, it can be written with arrow-syntax. Note that you
still need to annotate the return type explicitly.

```rust
def foo(a: u32, b: u32): u32 => a + b
```

Additionally, this can also be used for one-liner functions that don't return anything:

```rust
def foo(a: &u32, b: u32) => *a = b
```

### Variadic functions

These work similar to how variadics work in C. A variadic function is denoted by `...` as the
last argument, and cannot have default values for any of the arguments.

> [!WARNING]
> Due to their nature, variadic function calls are not type-checked and can type-issues.
> it's generally recommended to avoid using them.

> [!NOTE]
> Ocen does not currently have support for properly implementing variadic functions. They are
> currently here to serve as a way of writing Ocen wrappers for external variadic functions only.

```rust
import std::variadic::{ VarArgs }

def foo(n: u32, ...) {
   let va: VarArgs
   va.start(n)
   // Can't do much else except call other variadic functions,
   // usually implemented in some C library. Look below for how
   // to interop with C code.
   bar_variadic(va)
   va.end()
}
```


## Structs / Unions

Ocen provides `struct` and `union` compound data types (as in C). Anonymous struct/union
definitions are **not** allowed, and each must be declared separately. The defined is
referred to by it's name (without any qualifiers). No forward declaration of structs is needed.

Fields (and later methods) for a struct can be accessed using the `.` syntax

```rust
struct Foo {
   x: u32
   y: str
   a, b, c: u32   // Multi-field syntax is OK
   u: Bar         // Can use a type defined later
}

// Setting and accessing fields in a struct
let f: Foo = ...
f.x = 5
f.a = f.x

// A union takes as much space as it's largest member
union Bar {
   f: &Foo
   y: u32
   z: [u32; 10]
}
```

### Constructors

Structures can be constructed by using their name as a function. If doing so, every single
field of the structure needs to be specified.

> [!NOTE]
> Using constructors is not possible for unions, or when your structure contains
> any unions. This may be fixed in the future. In the meantime, the way to do this
> is to create an uninitialized variable and init the fields you want.

```rust
struct Vec2 {
   x, y: f32
}

let a = Vec2(1.0, 2.0)
let b = Vec2(x: 1.0, y: 2.0)  // Can use labels

// Manually
let c: Vec2
c.x = 1.0
c.y = 2.0
```


## Enums

Enums are defined with the `enum` keyword. All enum variants are namespaces to the parent
enum. Enums can only be compared with enums of the same type.

```rust
enum Size {
   Small
   Medium
   Big
}

enum WordsWithB {
   Big     // Doesn't clash with Size::Big
   Bug
   Bog
}

let x = Size::Big
let y: Size = Small   // Can be inferred if we have a hint
```

## Methods

All builtins, structs, enums and unions in Ocen can have methods associated with them.
These methods can either be static, or based on the instance of the object.

### Static Methods

A non-static method is one that doesn't take in `this` or `&this` as the first argument.
 It can only be called when qualified by the type name.

```rust
// Static method

def u32::from_str(s: str): u32 => ...

let x: u32 = u32::from_str("123")
```

### Instance Methods

An instance method **must** take in `this` or `&this` as it's first argument, depending on
whether it wants a copy or reference of the original object the method is being called on.
Generally, for most objects where methods modify the internal state of the object, or if the
object is dynamically allocated, it should take in `&this` by reference.

```rust
struct Foo {
   x: u32
}

def Foo::get_x(this): u32 => this.x             // OK to take in by value here
def Foo::set_x(&this, y: u32) => this.x = y     // Need to take in by reference

let f: Foo = ...
// Caller doesn't have to care about value/reference capture
let z = f.get_x()
f.set_x(z + 1)
```

Instance methods can also be treated as static methods if needed. In this case, the `this`
argument needs to be passed in explicitly by the caller.

```rust
let f: Foo = ...
let getter = Foo::get_x  // Use as static method, assign to func ptr
let setter = Foo::set_x
let z = getter(f)
setter(&f, z+1)  // Need to manually take reference here
```

### Dot Shorthand

In instance methods, you can use the `.foo` shorthand to refer to `this.foo`, to save
some typing. This can be used to access fields and methods from the `this` object.

```rust
// Rewritten with dot-shorthand
def Foo::get_x(this): u32 => .x
def Foo::set_x(&this, y: u32) => .x = y
```

## Templates

Ocen supports some basic templating (similar to C++). In particular, it does **not** support
interfaces/traits, and type-checks each different instantiation for the template separately.

This is an intentional choice to keep the language simpler - and allow the programmer to do
whatever they wish to do without having to convince the compiler something is valid through
complicated trait definitions.

Currently, there is no inference of template parameters, so the full templated name
needs to be specified when needed.

### Template functions

Simple example:

```rust
def swap<T>(a: &T, b: &T) {
   let tmp = *a
   *a = *b
   *b = tmp
}

let x = 5
let y = 10
swap<u32>(&x, &y)    // No inference, specify <u32> explicitly
```

A more nuanced example, showing possible errors:

```rust
def u32::hash(this): u32 => ...  // returns some hash

// Note how this assumes `v` has a `hash` method
def hasher<T>(v: T): u32 => v.hash() + 31415

// OK! `u32` has a `hash` method, which we defined above
let a = hasher<u32>(5)

// ERROR: Invalid
// This will error at `v.hash()` and say `str` has no member named `hash`
let b = hasher<str>("hi")
```

### Template Structs

An example of template structs:

```rust
struct Vector2D<T> {
   x, y: T
}

let a = Vector2D<u32>(1, 2)
let b = Vector2D<f32>(1.0, 2.0)

// Can have multiple template arguments
struct Item<K, V> {
   point: Vector2D<K>  // Can use nested templates
   item: FooBar<V>
}
```

All methods defined for templated structs **implicitly** get access to the template variables.
They do not need to be redefined.

```rust
struct Vector2D<T> {
   x, y: T
}

// Note that for `other`, we need to use the full templated type
def Vector2D::add(this, other: Vector2D<T>): Vector2D<T> {
   return Vector2D<T>(.x + other.x, .y + other.y)
}
```

## Explicit Namespaces

Each file in Ocen has it's own namespace for global-level declarations. Usually, we want to
organize code in different files to avoid polluting namespaces, but in some cases it can be
useful to have an explicit namespace in a file. We can do that with the `namespace` keyword.
Namespaces can be nest arbitrarily.

You can access elements from inside a namespace using the `::` syntax, similar to how we
access static methods on objects.

```rust
namespace foo {
   let a: i32 = 0

   namespace bar {
      def b(): u32 => 40
   }
}

def main() {
   let x = foo::a
   foo::bar::b()
}
```


## Attributes

Top-level declarations (functions/structs/etc) in Ocen can be tagged with different attributes.
The available attributes differ for each type of declaration, but are generally of the form:

Attributes are defined at the compiler-level, and it's not possible to create custom attributes
without changing the compiler.

```rust
[made_up_attr_0]
let X: i32

// All arguments **must** be string literals
[made_up_attr_1 "arg1"]
struct Bar { ... }

// Can use multiple attributes
[made_up_attr_0 "arg1"]
[made_up_attr_1 "arg1"]
[made_up_attr_2 "arg1" "arg2"]
def foo(): u32 => 0
```

### `extern` attribute, Interfacing with C functions

This attribute can be used with structs, enums, functions, variables and constants.
If used with no arguments, it assumes the name of the symbol matches the C one.
If used with methods, you should always provide the extern name.

Declarations tagged with the `extern` attribute cannot have definitions. This includes
functions and constants. These declarations do not result in any code generated, and are
simply a way of telling the compiler certain symbols exist, and how to type-check them.

```rust
[extern] let errno: i32          // No definition, C variable is called `errno` too
[extern "errno"] let ERR: i32    // Use `ERR` in Ocen, but `errno` in generated C
[extern "FILE"] struct File {}   // Don't need to specify any fields

[extern] def strcpy(a: str, b: str)  // Can lie about return type if we don't care
[extern] def malloc(sz: u16)         // Can lie about input types if C can cast implicitly
```

#### Complex Structure Bindings

If we wish to use C structs as more than just an opaque type, we need to tell ocen what the
fields are and what the types of those fields are. We only need to specify the ones we actually
care about - ocen does not check if these fields actually exist, but just takes your word for it.

> [!NOTE]
> Remember, this will not generate any code. It's simply for the type-checker.

```rust
[extern "Vector2D"]
struct Vec {
   x: f32   // `Vector2D` struct in C must have a field `x`
   // Only need to specify the fields you want to use
}

let v: Vec = ...
v.x   // Can use it in ocen now...
```

#### Method Bindings

Methods in Ocen are just normal functions that implicitly pass in the object as the first argument.
We can use this to bind external C functions as methods to external C types, creating a nicer
interface at the ocen level.

```rust
[extern "FILE"] struct File {}
// Bind extern as static function, with a default argument
[extern "fopen"] def File::open(fname: str, mode: str = "r"): &File
// Bind extern as instance function
[extern "fclose"] def File::close(&this)    // `&this` because `fclose(FILE*)`

// But we don't need to bind it as a method
[extern] def fread(a: untyped_ptr, x: u64, n: u64, f: &File)


let f = File::open("foo.txt")  // Uses default mode
fread(dummy, 1, 2, f)
f.close()
```

#### Enum Bindings

For enums, you need to bind each of the enum variants to an external symbol. Here's an example:

```rust
[extern "SDL_EventType"]
enum EventType {
   Quit = extern("SDL_QUIT")
   KeyDown = extern("SDL_KEYDOWN")
   KeyUp = extern("SDL_KEYUP")
   ...
}
```

#### Miscellaneous Binding Tips

As you might have noticed, we can bind whatever we want to `Ocen`, as long as we know
it's sound at the C level. It won't care as long as you don't. The names you provide
in the `extern` attribute are arbitrary strings - and this can be (ab)used in certain
scenarios to improve the usability when interfacing with C code.

One example is binding commonly used (non-enum) values to a function as an enum, to be able
to use the type inference in Ocen / make the code more readable. For instance:

```rust
// Raylib Bindings

[extern "int"]  // Not an enum, but we don't care
enum Key {
   A = extern("KEY_A")
   B = extern("(KEY_B * 1)")   // Can technically use any valid C expression here...
   ...
}
// Mark the input here as `Key`, since we know it's an int
[extern] def IsKeyPressed(key: Key): bool
[extern] def GetKeyPressed(): Key

def main() {
   IsKeyPressed(Key::A)
   IsKeyPressed(B)         // Inferred, without a global variable `B`

   // Can also print out for free...
   println(f"Key Pressed: {GetKeyPressed()}")
}
```

### `exits` attribute, Non-returning functions

The exits attribute can only be used for functions. It does not take in any arguments.
It is used to indicate that a function does not ever return. It is used by the compiler when
doing return analysis.

```rust
[exits]
def foo() {
   std::exit(1)
}

def bar(): u32 {
   foo()
   // If foo was not marked as `exits`, the compiler would complain
   // about `bar()` not always returning a `u32`.
}
```

### `export` attribute, Re-exporting symbols

[[TODO: Module system / importing]]

The `export` attribute is only available for global import statement. It takes in no
arguments. It tells the compiler to re-export the imported symbol(s) from the current
namespace.

This is most useful when writing library code, to expose functions defined in nested
modules from the top-level file.

```rust
///////////// file: src/bar/bar_impl.oc
// The actual function
def do_bar(): u32 => 30

///////////// file: src/bar/mod.oc
// Re-export it
[export] import .bar_impl::{ do_bar }


///////////// file src/main.oc
// Can import `do_bar` from `bar`
import .bar::{ do_bar }
```

### `operator` attribute, Operator Overloading

The `operator` attribute is used for operator overloading, and only applies to functions.
It takes in exactly one argument, representing the operator we want to overload with the
current function. One function can be used to overload multiple operators by specifying
separate attributes if needed.

```rust
struct Vector2D {
   x, y: u32
}

[operator "+"]  // Operator **must** be in a string literal
def Vector2D::add(this, other: Vector2D): Vector2D {
   return Vector2D(.x + other.x, .y + other.y)
}

let z = Vector2D(0,0) + Vector2D(1,2)  // Now allowed
```

Every overload defined for an operator needs to have unique input signature. For instance,
it is **not allowed** to have two overloads for `Foo + u32` (even if they result in different
types). It **is** allowed to have `Foo + u32` and `Foo + i32`, etc.

For a function to overload a certain operator, it must satisfy the requirements for that operator.
These are listed below for all the currently overridable operators. For each of the operations
below, `x`, `y` and `z` are the first, second, and third arguments respectively (where applicable).

- `+` : 2 arguments to the function (`x + y`)
- `-` : 1/2 arguments to the function (`-x` / `x - y`)
- `*` : 2 arguments to the function (`x * y`)
- `/` : 2 arguments to the function (`x / y`)
- `<<` : 2 arguments to the function (`x << y`)
- `>>` : 2 arguments to the function (`x >> y`)
- `&` : 1/2 arguments to the function (`&x` / `x & y`)
- `|` : 2 arguments to the function (`x | y`)
- `+=` : 2 arguments to the function, first is pointer (`x += y`)
- `-=` : 2 arguments to the function, first is pointer (`x -= y`)
- `*=` : 2 arguments to the function, first is pointer (`x *= y`)
- `/=` : 2 arguments to the function, first is pointer (`x /= y`)
- `<<=` : 2 arguments to the function, first is pointer (`x <<= y`)
- `>>=` : 2 arguments to the function, first is pointer (`x >>= y`)
- `[]` : 2 arguments to the function (`x[y]`)
- `%` : 2 arguments to the function (`x[y]`)
- `in` : 2 arguments to the function, and returns a bool (`y in x`) [**Look at note below**]
- `not` : 1 arguments to the function, and returns a bool (`not x`)
- `==` : 2 arguments to the function, and returns a bool (`x == y`)
- `!=` : 2 arguments to the function, and returns a bool (`x != y`)
- `?` : 1 argument to the function, and returns a bool (`x?`)
- `[]=` : 3 arguments to the function (`x[y] = z`)

> [!NOTE]
> For the `in` operator, the order of arguments is swapped. This is because often, the
> corresponding method we want to bind takes the value we are searching for as the second argument
> (and the instance variable `this` as the first). Example usage:
>
> ```rust
> [operator "in"]
> def StringHashMap::contains(&this, s: str): bool => ...
>
> // Usage
> let h: StringHashMap
> // Note how the string here naturally wants to be the second argument
> if "foo" in h { ... }
> ```



### `atomic` attribute, Atomic Variables

The atomic attribute applies to global variables / struct fields. It takes in no arguments.
It inidicates that the variable is atomic, and prepends the declaration in C with `_Atomic`.

```rust
[atomic] let counter: i32 = 0

struct Foo {
   [atomic]
   x: i32
}
```

### `variadic_format` attribute, Format Strings as arguments

The `variadic_format` attribute only applies to variadic functions. It takes in no arguments.
It tells the compiler that the function being tagged expects a variadic format-like string
as it's last argument (similar to `printf` and `fprintf`).

When such a function is called with a format-string as the last argument, instead of creating
an allocated formating string on the heap, it converts it to the variadic arguments.

> [!NOTE]
> The last non-variadic argument for a function being tagged must be of type `str`.

```rust
[variadic_format]
def foo(fmt: str, ...)

// This call:
foo(`Hello {1+2:.1f} from {"ocen"}`)
// Automatically gets expanded to:
foo("Hello %.12f from %s", 1+2, "ocen")  // No allocation!
```


### `formatting` attribute, Basic Formatting of custom structs

The `formatting` attribute only applies to structs. It takes in 2 arguments, and is used by
format-strings to provide a (very minimal) way of formatting some basic structs.
The arguments are:

1. A string representing the format specifier(s) to add to the format string
2. A string representing what the arguments to the format-string should be.
   In this string, all used of the character `$` will be replaced by the original
   expression in the format-string.

This is better shown by example:

```rust
[formatting "Foo(%u)" "$.x"]
struct Foo {
   x: u32
}

let f: Foo
// This line:
println(`f has the value {f}`)
// Automatically gets expanded to:
println("f has the value Foo(%u)", (f).x)
```

Here's a more complex example:

```rust
// 1. Note how the first argument is an arbitrary string, with multiple specifiers
// 2. We can use `$` multiple times, and can comma-separate multiple arguments
//    In this case, note that `%.*s` takes in 2 arguments
[formatting "SV(size=%u, data='%.*s')" "$.size, $.size, $.data"]
struct StringView {
   data: str
   size: u32
}
let s: StringView
// This line:
println(`s = {s}`)
// Automatically gets expanded to:
println("s = SV(size=%u, data='%.*s')", (s).size, (s).size, (s).data)
```

> [!NOTE]
> In complex cases, we evaulate the expression passed to the format string multiple times. To avoid potential
> bugs due to unwanted side-effects, the compiler prohibits you from using arbitrary expressions that result
> in the `StringView` (or other custom) type. The recommendation is to save the value to a variable, and pass
> just the variable to the format string as an expression.