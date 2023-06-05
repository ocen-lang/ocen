# big aecor rewrite

What works so far:
   - full lexer
   - parser for only builtin types + basic imports (look in `test/test.ae`)
   - no typechecker/codegen yet. the corresponding files in `compiler/` are just copied over from the `aecor` repo and are *not* compiled yet.

## Build

```
$ aecor compiler/main.ae
# Run to see if everything parses:
$ ./out test/test.ae
```