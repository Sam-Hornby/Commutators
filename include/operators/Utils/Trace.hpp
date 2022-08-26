#pragma once
#include <Logging.hpp>
#include <chrono>

namespace operators {

namespace {

using TimePeriod = std::chrono::duration<double, std::chrono::microseconds::period>;
long depth = 0;

}

void log_end(const std::string & name,
             const std::chrono::time_point<std::chrono::high_resolution_clock> & start,
             long & depth) {
    auto end = std::chrono::high_resolution_clock::now();
    auto length = TimePeriod(end - start);
    spdlog::trace("{}End: {} --> Stats: [ duration = {} \u00b5s ]", std::string(depth, ' '), name, length.count());
    --depth;
}

template <class F>
typename std::invoke_result<F>::type trace(const std::string & name, F f) {
    depth++;
    spdlog::trace("{}Begin: {}", std::string(depth, ' '),name);
    auto start = std::chrono::high_resolution_clock::now();
    using RetType = typename std::invoke_result<F>::type;
    if constexpr (std::is_same<RetType, void>::value) {
        f();
        log_end(name, start, depth);
    } else {
        RetType x = f();
        log_end(name, start, depth);
        return x;
    }
}

#define TRACE(operation) trace(#operation, [&] () {\
  return operation ;\
});\

}



