#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <iostream> // TODO remove

#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) {
    pid_ = pid;
    // Get User
    user_ = LinuxParser::User(pid_);

    // Get Command
    command_ = LinuxParser::Command(pid_);
}

int Process::Pid() { return pid_; }

float Process::CpuUtilization() {
    long prc = LinuxParser::ActiveJiffies(pid_);
    vector<string> values = LinuxParser::CpuUtilization();
    long active = stol(values[0]) + stol(values[1]) + stol(values[2]) + stol(values[5]) + stol(values[6]) + stol(values[7]);
    cpu_use_ = (float)prc / active;
    return cpu_use_;
}

string Process::Command() { return command_; }

string Process::Ram() {
    return LinuxParser::Ram(pid_);
}

string Process::User() { return user_; }

long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

bool Process::operator<(Process const& a) const {
    return cpu_use_ > a.cpu_use_;
    // return true;
}