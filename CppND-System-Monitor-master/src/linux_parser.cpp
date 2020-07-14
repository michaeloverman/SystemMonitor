#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using namespace std;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          filestream.close();
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
// CORRECTED to get actual kernel info, as described in video
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  stream.close();
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), ::isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  string line, key, value;
  vector<float> vals {};
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    int count = 0;
    while (count < 2) { // Total memory, free memory
      std::getline(stream, line);
      std::istringstream linestream(line);
      while (linestream >> key >> value) { // Get label and value, skip units
        vals.push_back(stof(value));  // Convert to float and save
      }
      count++;
    }
  }
  stream.close();
  int used = vals[0] - vals[1]; // Used mem = total - free
  return used / vals[0]; // Return percentage of total used
}

long LinuxParser::UpTime() {
  string line, up;
  float uptime;
  ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    getline(stream, line);
    istringstream linestream(line);
    linestream >> up;
    uptime = stof(up);
  }
  stream.close();
  return (long)uptime;
}

long LinuxParser::ActiveJiffies(int pid) {
  string line, junk, value;
  long sum = 0L;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    int count = 0;
    while (count++ < 13) {
      linestream >> junk;
    }
    while (count++ < 17) {
      linestream >> value;
      sum += stol(value);
    }
  }
  stream.close();
  return sum;
}

vector<string> LinuxParser::CpuUtilization() {
  vector<string> values {};
  std::string line, cpu, one;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> cpu;
      int count = 0;
      while (count < 10) {
        linestream >> one;
        stream.close();
        values.push_back(one);
        count++;
      }
      return values;
    }
  }
  return values; // If this happens, it will be an empty vector, but it shouldn't happen
}

int LinuxParser::TotalProcesses() {
  string line, leader, value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> leader;
      if (leader == "processes") {
        linestream >> value;
        stream.close();
        return stoi(value);
      }
    }
  }
  return -1; // Junk value should not happen
}

int LinuxParser::RunningProcesses() {
  string line, leader, value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> leader;
      if (leader == "procs_running") {
        linestream >> value;
        stream.close();
        return stoi(value);
      }
    }
  }
  return -1; // Junk value, should not happen
}

string LinuxParser::Command(int pid) {
  string line, leader, value;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      stream.close();
      return line;
    }
  }
  return "commandline file not found";
}

string LinuxParser::Ram(int pid) {
  string line, leader, value;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> leader;
      if (leader == "VmSize:") {
        linestream >> value;
        stream.close();
        int mb = stol(value) / 1024;
        return to_string(mb);
      }
    }
  }
  return "0";
}

string LinuxParser::Uid(int pid) {
  string line, leader, uid;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> leader;
      if (leader == "Uid:") {
        linestream >> uid;
        stream.close();
        return uid;
      }
    }
  }
  return "-1"; // Junk value - should not happen
}

string LinuxParser::User(int pid) {
  string uid = Uid(pid);
  string line, user, x, value;
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user >> x >> value;
      if (value == uid) {
        stream.close();
        return user;
      }
    }
  }
  return "FREAD"; // Junk value, should not happen
}

long LinuxParser::UpTime(int pid) {
  string line, value;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      for (int i = 0; i < 22; i++) {
      	linestream >> value;
      }
      if (value != "") {
        stream.close();
        return UpTime() - stol(value) / sysconf(_SC_CLK_TCK);
      }
    }
  }
  return 12L; // Junk value - should only happen if something goes wrong above
}