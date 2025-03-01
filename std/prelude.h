#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdlib.h>

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

//// Backtraces

volatile static const char *__oc_bt[] = {0};
volatile u64 __oc_bt_idx = 0;

#define _WITH_BT(s, ...)      \
  __oc_bt[__oc_bt_idx++] = s; \
  __VA_ARGS__;                \
  (void)__oc_bt_idx--;

void dump_backtrace() {
  if (__oc_bt_idx == 0) {
    return;
  }
  fprintf(stderr, "--------------------------------------------------------------------------------\n");
  fprintf(stderr, "Backtrace:\n");
  for (u64 i = 0; i < __oc_bt_idx; i++) {
    fprintf(stderr, "  => %s\n", __oc_bt[i]);
  }

  fprintf(stderr, "--------------------------------------------------------------------------------\n");
}

/// End backtraces

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
