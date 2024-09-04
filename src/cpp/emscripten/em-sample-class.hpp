#include "em-main.h"
#include "sample-class.hpp"

SampleClass sampleClass;
class EmSampleClass
{
public:
    int test_fn_1(int x)
    {
        int res = 0;
        bool done = false;
        queue.proxyAsync(main_thread, [&]()
                         {
                             std::unique_lock<std::mutex> lock(mutex);
                             sampleClass.test_fn_1(x);
                             res = 9;
                             done = true;
                             printf("test_fn_1: %d\n", x);
                             lock.unlock();
                             //
                         });
        while (!done)
        {
            emscripten_sleep(0);
        }
        return res;
    }
};
EMSCRIPTEN_BINDINGS(EmSampleClass)
{
    emscripten::class_<EmSampleClass>("EmSampleClass")
        .constructor<>()
        .function("test_fn_1", &EmSampleClass::test_fn_1);
}