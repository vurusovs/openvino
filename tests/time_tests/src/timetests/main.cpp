// Copyright (C) 2018-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "cli.h"
#include "timer.h"
#include "statistics_writer.h"
#include "utils.h"

#include <iostream>

#include <inference_engine.hpp>

using namespace InferenceEngine;

/**
 * @brief Function that contain executable pipeline which will be called from
 * main(). The function should not throw any exceptions and responsible for
 * handling it by itself.
 */
int runPipeline(const std::string &model, const std::string &device) {
  auto pipeline = [](const std::string &model, const std::string &device) {
    Core ie;
    CNNNetwork cnnNetwork;
    ExecutableNetwork exeNetwork;
    InferRequest inferRequest;
    size_t batchSize = 0;

    {
      SCOPED_TIMER(first_inference_latency);
      {
        SCOPED_TIMER(load_plugin);
        ie.GetVersions(device);
      }
      {
        SCOPED_TIMER(create_exenetwork);
        if (TimeTest::fileExt(model) == "blob") {
          SCOPED_TIMER(import_network);
          exeNetwork = ie.ImportNetwork(model, device);
        }
        else {
          {
            SCOPED_TIMER(read_network);
            cnnNetwork = ie.ReadNetwork(model);
            batchSize = cnnNetwork.getBatchSize();
          }

          {
            SCOPED_TIMER(load_network);
            exeNetwork = ie.LoadNetwork(cnnNetwork, device);
          }
        }
      }
    }
  };

  try {
    SCOPED_TIMER(full_run);
    pipeline(model, device);
  } catch (const InferenceEngine::Exception &iex) {
    std::cerr
        << "Inference Engine pipeline failed with Inference Engine exception:\n"
        << iex.what();
    return 1;
  } catch (const std::exception &ex) {
    std::cerr << "Inference Engine pipeline failed with exception:\n"
              << ex.what();
    return 2;
  } catch (...) {
    std::cerr << "Inference Engine pipeline failed\n";
    return 3;
  }
  return 0;
}

/**
 * @brief Parses command line and check required arguments
 */
bool parseAndCheckCommandLine(int argc, char **argv) {
  gflags::ParseCommandLineNonHelpFlags(&argc, &argv, true);
  if (FLAGS_help || FLAGS_h) {
    showUsage();
    return false;
  }

  if (FLAGS_m.empty())
    throw std::logic_error(
        "Model is required but not set. Please set -m option.");

  if (FLAGS_d.empty())
    throw std::logic_error(
        "Device is required but not set. Please set -d option.");

  if (FLAGS_s.empty())
    throw std::logic_error(
        "Statistics file path is required but not set. Please set -s option.");

  return true;
}

/**
 * @brief Main entry point
 */
int main(int argc, char **argv) {
  if (!parseAndCheckCommandLine(argc, argv))
    return -1;

  StatisticsWriter::Instance().setFile(FLAGS_s);
  for (int i = 0; i < FLAGS_niter; ++i) {
     auto _status = runPipeline(FLAGS_m, FLAGS_d);
     if (_status != 0)
         return _status;
  }
  StatisticsWriter::Instance().writeToFile();
  return 0;
}