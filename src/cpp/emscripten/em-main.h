#ifndef EM_MAIN_H_
#define EM_MAIN_H_

#include <emscripten/emscripten.h>
#include <emscripten/proxying.h>
#include <emscripten/bind.h>

#include <mutex>

bool is_running = true;
emscripten::ProxyingQueue queue;
std::mutex mutex;
pthread_t main_thread = 0;

#endif