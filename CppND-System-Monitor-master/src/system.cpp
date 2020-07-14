#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"

using namespace std;

Processor& System::Cpu() {
    cpu_ = Processor();
    return cpu_;
}

vector<Process>& System::Processes() {
    vector<int> pids = LinuxParser::Pids();
    for (int p : pids) {
        Process proc(p);
        processes_.push_back(proc);
    }
    sort(processes_.begin(), processes_.end());
    return processes_;
}

std::string System::Kernel() {
    if (kernal_string_ == "") {
        kernal_string_ = LinuxParser::Kernel();
    }
    return kernal_string_;
}

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() {
    if (os_string_ == "") {
        os_string_ = LinuxParser::OperatingSystem();
    }
    return os_string_;
}

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime(); }