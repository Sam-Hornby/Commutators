#pragma once

#include "spdlog/spdlog.h"
#include <iostream>

namespace operators {

namespace logging {

spdlog::level::level_enum getLevelFromString(const std::string &s) {
  if (s == "TRACE") {
    return spdlog::level::trace;
  }
  if (s == "DEBUG") {
    return spdlog::level::debug;
  }
  if (s == "INFO") {
    return spdlog::level::info;
  }
  if (s == "WARN") {
    return spdlog::level::warn;
  }
  if (s == "ERR") {
    return spdlog::level::err;
  }
  if (s == "CRITICAL") {
    return spdlog::level::critical;
  }
  assert(s == "OFF");
  return spdlog::level::off;
}

struct LoggingContext {
  LoggingContext () {
    auto log_level = std::getenv("OPERATORS_LOG_LEVEL");
    spdlog::set_level(getLevelFromString(log_level ? log_level : "OFF"));    
  }
};

LoggingContext global_context = LoggingContext{};

}} // namespace operators::logging
