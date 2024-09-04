#include "em-main.h"
#include "em-llama-cpp.hpp"

extern "C"
{
    EMSCRIPTEN_KEEPALIVE
    void em_exit()
    {
        is_running = false;
    }
}

int main(int argc, char **argv)
{
    main_thread = pthread_self();
    // printf("main ptr: %lu\n", main_thread);

    // Tell javascript-land we are ready
    EM_ASM(
        // clang-format off
        self.postMessage({em_cmd: 'main_thread_ready'});
        // clang-format on
    );

    // When compiled with PROXY_TO_PTHREAD flag, the main function
    // runs in a separate web worker thread but calls to code
    // created using EM_BIND will run on the main browser thread
    // and make the UI unresponsive.
    // To fix this, we use the emscripten proxy API to queue calls
    // onto the main web worker thread instead.
    pthread_t thread;
    while (is_running)
    {
        queue.execute();
        sched_yield();
        emscripten_sleep(0);
    }
    return 0;
}
