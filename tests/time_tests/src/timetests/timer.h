// Copyright (C) 2018-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <chrono>
#include <string>
#include <fstream>
#include <memory>

#include "statistics_writer.h"


namespace TimeTest {
using time_point = std::chrono::high_resolution_clock::time_point;

/** Encapsulate time measurements.
Object of a class measures time at start and finish of object's life cycle.
When destroyed, reports duration.
*/
class Timer {
private:
  std::string name;
  time_point start_time;

public:
  /// Constructs Timer object and measures start time.
  Timer(const std::string &timer_name) {
    name = timer_name;
    start_time = std::chrono::high_resolution_clock::now();
  }

  /// Destructs Timer object, measures duration and reports it.
  ~Timer() {
    float duration = std::chrono::duration_cast<std::chrono::microseconds>(
                         std::chrono::high_resolution_clock::now() - start_time)
                         .count();
    StatisticsWriter::Instance().write({name, duration});
  }
};

#define SCOPED_TIMER(timer_name) TimeTest::Timer timer_name(#timer_name);

} // namespace TimeTest