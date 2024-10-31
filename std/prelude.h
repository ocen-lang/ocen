#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>
#include <inttypes.h>

#ifndef OC_NO_BACKTRACE
#include <execinfo.h>
#endif

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

void dump_backtrace() {
#ifndef OC_NO_BACKTRACE
  void *array[40];
  size_t size = backtrace(array, 40);
  char **strings = backtrace_symbols(array, size);
  printf("\nBacktrace:\n");
  for (size_t i = size-1; i > 0; i--) {
    printf("%s\n", strings[i]);
  }
  free(strings);
#endif
}

#ifdef __APPLE__
  #define oc_trap __builtin_debugtrap
#else
  #define oc_trap __builtin_trap
#endif

void ae_assert_fail(char *dbg_msg, char *msg) {
  dump_backtrace();
  printf("--------------------------------------------------------------------------------\n");
  printf("%s\n", dbg_msg);
  if (msg) {
    printf("  Message: %s\n", msg);
  }
  printf("--------------------------------------------------------------------------------\n");
  fflush(stdout);
  oc_trap();
}
