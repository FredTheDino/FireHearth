extern "C" {
#include "stdio.h"
#include "stdarg.h"
}

// Debug functions
#define LOG_MSG(fmt) __debug_log("LOG", __FILE__, __LINE__, fmt)
#define LOG(fmt, ...) __debug_log("LOG", __FILE__, __LINE__, fmt, __VA_ARGS__)
#define ERR_MSG(fmt) __debug_log("ERR", __FILE__, __LINE__, fmt)
#define ERR(fmt, ...) __debug_log("ERR", __FILE__, __LINE__, fmt, __VA_ARGS__)
static void
__debug_log(const char* type, const char* file, int line, const char *fmt, ...) {
    // TODO: Log file with timestamps?
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "%s|%d| [%s]: ", file, line, type);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
}

#define HALT_AND_CATCH_FILE ((int*)(void*)0)[0] = 0;
#define ASSERT(expr) __assert(__FILE__, __LINE__, "\""#expr"\"", expr)
static void
__assert(const char* file, int line, const char* expr, bool assumed) {
    if (assumed) return;
    __debug_log("!ASSERT!", file, line, expr);
    HALT_AND_CATCH_FILE;
}
#define UNREACHABLE __debug_log("UNREACHABLE", __FILE__, __LINE__, "Reached unreachable code"); HALT_AND_CATCH_FILE;

#define CHECK(expr) __check(__FILE__, __LINE__, "\""#expr"\"", expr)
static void
__check(const char* file, int line, const char* expr, bool assumed) {
    if (assumed) return;
    __debug_log("?CHECK?", file, line, expr);
}

#include "base.h"
#define OPENGL
#define SDL
#include "renderer/command.cpp"
#include "platform/input.h"
#include "platform/input.cpp"

Input::Mapping mapping = {};

#ifdef SDL
// TODO(ed): Better job of abstracting out SDL.
#   include "platform/input_sdl.cpp"
#else
#   error "No other platform layer than SDL supported."
#endif

int
main(int argc, char **argv) {

    ASSERT(Renderer::init("Hello", 500, 500));

    using namespace Input;
    CHECK(add(&mapping, K(a), Player::P1, Name::LEFT));
    CHECK(add(&mapping, K(d), Player::P1, Name::RIGHT));
    CHECK(add(&mapping, K(ESCAPE), Player::P1, Name::QUIT));

    while (SDL::running) {
        SDL::poll_events();

        if (value(&mapping, Player::ANY, Name::QUIT))
            SDL::running = false;
        Renderer::clear();
        Renderer::blit();
        // TODO: Input loop.
    }

	return 0;
}
