#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>
#include <inttypes.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

char* format_string(const char* format, ...) {
  va_list args;
  va_start(args, format);
  int size = vsnprintf(NULL, 0, format, args);
  va_end(args);
  va_start(args, format);
  char* s = (char*)calloc(1, size + 1);
  vsprintf(s, format, args);
  s[size] = '\0';
  va_end(args);
  return s;
}

void ae_assert(int cond, char *dbg_msg, char *msg) {
  if (!cond) {
    printf("--------------------------------------------------------------------------------\n");
    printf("%s\n", dbg_msg);
    if (msg) {
      printf("  Message: %s\n", msg);
    }
    printf("--------------------------------------------------------------------------------\n");
    fflush(stdout);
    __builtin_trap();
  }
}