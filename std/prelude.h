#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>

#ifndef OC_NO_BACKTRACE
#include <stdlib.h>
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

const char* __asan_default_options() { return "detect_leaks=0"; }

void dump_backtrace() {
#ifndef OC_NO_BACKTRACE
  void *array[40];
  size_t size = backtrace(array, 40);
  char **strings = backtrace_symbols(array, size);
  fprintf(stderr, "\nBacktrace:\n");
  for (size_t i = size-1; i > 0; i--) {
    fprintf(stderr, "%s\n", strings[i]);
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
  fprintf(stderr, "--------------------------------------------------------------------------------\n");
  fprintf(stderr, "%s\n", dbg_msg);
  if (msg) {
    fprintf(stderr, "  Message: %s\n", msg);
  }
  fprintf(stderr, "--------------------------------------------------------------------------------\n");
  fflush(stdout);
  oc_trap();
}
