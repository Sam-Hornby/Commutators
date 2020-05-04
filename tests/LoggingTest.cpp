#include "Logging.hpp"
#define BOOST_TEST_MODULE Logging
#include <boost/test/included/unit_test.hpp>

using namespace operators;

BOOST_AUTO_TEST_CASE(Logging) {
  spdlog::info("Info");
  spdlog::debug("Debug");
  spdlog::trace("Trace");
  spdlog::warn("Warn");
  spdlog::error("Err");
  spdlog::critical("critical");
}
 
