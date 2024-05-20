/**
 * This is a simple interface to register a main loop function.
 * It's written at the C level since we want some compile-time
 * conditional logic dependong on the target platform, which
 * we cannot do natively in Ocen right now.
*/

// NOTE: Emscripten expects a callback that doesn't return anything,
//       but we want to be able to return a boolean to indicate when
//       the main loop should stop. We'll use a wrapper function to
//       handle this, and store the actual callback in a global variable.


#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>

_Bool (*callback)(void) = NULL;
// EMS no-return callback wrapper
void ems_callback() {
    if (callback) callback();
}
#endif

void c_set_main_loop(_Bool (*func)(void)) {
#ifdef __EMSCRIPTEN__
    callback = func;
    emscripten_set_main_loop(ems_callback, 0, 1);
#else
    while (func()) {}
#endif
}
