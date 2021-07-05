// Copyright (C) 2018-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "timetests_helper/timer.h"
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "statistics_writer.h"

namespace MemoryTest {
#ifdef _WIN32
size_t getVmSizeInKB() {
                // TODO rewrite for Virtual Memory
                PROCESS_MEMORY_COUNTERS pmc;
                pmc.cb = sizeof(PROCESS_MEMORY_COUNTERS);
                GetProcessMemoryInfo(GetCurrentProcess(),&pmc, pmc.cb);
                return pmc.WorkingSetSize;
	    }
#else
/// Parses number from provided string
static int parseLine(std::string line) {
    std::string res = "";
    for (auto c: line)
        if (isdigit(c))
            res += c;
    if (res.empty())
        // If number wasn't found return -1
        return -1;
    return std::stoi(res);
}

size_t getSystemDataByName(char *name){
    FILE* file = fopen("/proc/self/status", "r");
    size_t result = 0;
    if (file != nullptr) {
        char line[128];

        while (fgets(line, 128, file) != NULL) {
            if (strncmp(line, name, strlen(name)) == 0) {
                result = parseLine(line);
                break;
            }
        }
        fclose(file);
    }
    return result;
}

size_t getVmSizeInKB() {return getSystemDataByName((char*) "VmSize:");}
size_t getVmPeakInKB() {return getSystemDataByName((char*) "VmPeak:");}
size_t getVmRSSInKB() {return getSystemDataByName((char*) "VmRSS:");}
size_t getVmHWMInKB() {return getSystemDataByName((char*) "VmHWM:");}
size_t getThreadsNum() {return getSystemDataByName((char*) "Threads:");}

#endif


MemoryCounter::MemoryCounter(const std::string &mem_counter_name) {
  name = mem_counter_name;
  StatisticsWriter::Instance().addMemCounter(name);
}

MemoryCounter::~MemoryCounter() {
  std::vector<size_t> memory_measurements = {getVmRSSInKB(), getVmHWMInKB(), getVmSizeInKB(),
                                             getVmPeakInKB(), getThreadsNum()};
  StatisticsWriter::Instance().deleteMemCounter({name, memory_measurements});
}

} // namespace TimeTest