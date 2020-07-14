#include "processor.h"

float Processor::Utilization() {
    std::vector<std::string> values = LinuxParser::CpuUtilization();
    long idle = stol(values[3]) + stol(values[4]);
    long active = stol(values[0]) + stol(values[1]) + stol(values[2]) + stol(values[5]) + stol(values[6]) + stol(values[7]);
    return (float)active / (active + idle);
}