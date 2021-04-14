// Copyright (C) 2018-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <vector>
#include <map>
#include <numeric>
#include <algorithm>

/**
 * @brief Class response for writing provided statistics
 *
 * Object of the class is writing provided statistics to a specified
 * file in YAML format.
 */
class StatisticsWriter {
private:
  std::ofstream statistics_file;
  std::map<std::string, std::vector<float>> stats;

  StatisticsWriter() = default;
  StatisticsWriter(const StatisticsWriter &) = delete;
  StatisticsWriter &operator=(const StatisticsWriter &) = delete;

public:
  /**
   * @brief Creates StatisticsWriter singleton object
   */
  static StatisticsWriter &Instance() {
    static StatisticsWriter writer;
    return writer;
  }

  /**
   * @brief Specifies, opens and validates statistics path for writing
   */
  void setFile(const std::string &statistics_path) {
    statistics_file.open(statistics_path);
    if (!statistics_file.good()) {
      std::stringstream err;
      err << "Statistic file \"" << statistics_path
          << "\" can't be used for writing";
      throw std::runtime_error(err.str());
    }
  }

  /**
   * @brief Writes provided statistics in YAML format.
   */
  void write(const std::pair<std::string, float> &record) {
    stats[record.first].push_back(record.second);
  }

   /**
   * @brief Writes provided statistics in YAML format.
   */
  void writeToFile() {
    if (!statistics_file)
      throw std::runtime_error("Statistic file path isn't set");
    for (auto item: stats) {
        auto sum = std::accumulate(item.second.begin(), item.second.end(), 0.0);
        auto mean = sum / item.second.size();

        std::vector<double> diff(item.second.size());
        std::transform(item.second.begin(), item.second.end(), diff.begin(), [mean](double x) { return x - mean; });
        double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
        double stdev = std::sqrt(sq_sum / item.second.size());
        statistics_file << item.first << ": " << "\n"
                        << "  avg: " << mean << "\n"
                        << "  stdev: " << stdev << "\n"
                        << "---" << "\n"
                        << "unit_of_measurement: microsecond" << "\n";
    }
  }
};
