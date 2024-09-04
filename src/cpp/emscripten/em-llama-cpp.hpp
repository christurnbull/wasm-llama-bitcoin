#include "em-main.h"
#include "llama-cpp.hpp"

class EmLlamaCpp
{
public:
    void init(int32_t n_threads)
    {
        bool done = false;
        queue.proxyAsync(main_thread, [&]()
                         {
                             std::unique_lock<std::mutex> lock(mutex);
                             llamaCpp.init(n_threads);
                             done = true;
                             lock.unlock();
                             //
                         });
        while (!done)
        {
            emscripten_sleep(0);
        }
    }

    void destroy()
    {
        bool done = false;
        queue.proxyAsync(main_thread, [&]()
                         {
                             std::unique_lock<std::mutex> lock(mutex);
                             llamaCpp.destroy();
                             done = true;
                             lock.unlock();
                             //
                         });
        while (!done)
        {
            emscripten_sleep(0);
        }
    }

    void load_model(const std::string &model_id)
    {
        bool done = false;
        queue.proxyAsync(main_thread, [&]()
                         {
                             std::unique_lock<std::mutex> lock(mutex);
                             llamaCpp.load_model(model_id);
                             done = true;
                             lock.unlock();
                             //
                         });
        while (!done)
        {
            emscripten_sleep(0);
        }
    }

    std::string predict(const std::string &prompt)
    {
        std::string pred;
        bool done = false;
        queue.proxyAsync(main_thread, [&]()
                         {
                             std::unique_lock<std::mutex> lock(mutex);
                             pred = llamaCpp.predict(prompt);
                             done = true;
                             lock.unlock();
                             //
                         });
        while (!done)
        {
            emscripten_sleep(0);
        }
        return pred;
    }

private:
    LlamaCpp llamaCpp;
};

EMSCRIPTEN_BINDINGS(EmLlamaCpp)
{
    emscripten::class_<EmLlamaCpp>("EmLlamaCpp")
        .constructor<>()
        .function("init", &EmLlamaCpp::init)
        .function("destroy", &EmLlamaCpp::destroy)
        .function("load_model", &EmLlamaCpp::load_model)
        .function("predict", &EmLlamaCpp::predict);
}