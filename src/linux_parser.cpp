#include <iostream>
#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <numeric>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      CleanString_(line, true);
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
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
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  float memory_utilization = 0, active_memory, inactive_memory, total_memory;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    string line;
    while (std::getline(stream, line)) {
      string key, value;
      CleanString_(line, true);
      std::istringstream linestream(line);
      linestream >> key >> value;
      std::replace(value.begin(), value.end(), '_', ' ');
      value = std::regex_replace(value, std::regex("kB"), "");
      if (key == "Active") {
        active_memory = std::stof(value);
      } else if (key == "Inactive") {
        inactive_memory = std::stof(value);
      } else if (key == "MemTotal") {
        total_memory = std::stof(value);
      }
    }
    memory_utilization = (active_memory + inactive_memory) / total_memory;
  }
  return memory_utilization;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  long uptime = 0;
  string line, uptime_string;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime_string;
    uptime = stol(uptime_string);
  }
  return uptime;
}

long LinuxParser::SystemStartTime() {
  long uptime = 0;
  time_t current_time = 0;
  string line, uptime_string;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  uptime = LinuxParser::UpTime();
  std::time(&current_time);
  return current_time - uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return ActiveJiffies() + IdleJiffies();
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  long utime = 0, stime = 0, cutime = 0, cstime = 0;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    string line;
    std::getline(stream, line);
    CleanString_(line, false);
    std::istringstream linestream(line);
    vector<string> values(std::istream_iterator<string>(linestream), {});
    utime = stol(values[13]);
    stime = stol(values[14]);
    cutime = stol(values[15]);
    cstime = stol(values[16]);
  }
  return (utime + stime + cutime + cstime) / sysconf(_SC_CLK_TCK);
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    string line;
    std::getline(stream, line);
    CleanString_(line, false);
    line = std::regex_replace(line, std::regex("cpu "), "");
    std::istringstream linestream(line);
    vector<long> values(std::istream_iterator<long>(linestream), {});
    long sum = std::accumulate(values.begin(), values.end(), 0);
    sum -= values[4];
    sum -= values[5];
    return sum;
  }
  return 0;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  long idle_time = 0, iowait = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    string line;
    std::getline(stream, line);
    CleanString_(line, false);
    line = std::regex_replace(line, std::regex("cpu "), "");
    std::istringstream linestream(line);
    vector<long> values(std::istream_iterator<long>(linestream), {});
    idle_time = values[4];
    iowait = values[5];
  }
  return idle_time + iowait;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line;
  vector<string> values = {};
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream  linestream(line);
    string value;
    while (linestream >> value) {
      values.emplace_back(value);
    }
  }
  return values;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  return LinuxParser::Pids().size();
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  int counter = 0;
  for (int pid : LinuxParser::Pids()) {
    if (LinuxParser::Command(pid).length() > 1) {
      counter++;
    }
  }
  return counter;
}

// TODO: Read and return theouble command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string cmdline;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, cmdline);
  }
  return cmdline;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string line, memory_size;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      string  key, value;
      CleanString_(line, true);
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmSize") {
        std::replace(value.begin(), value.end(), '_', ' ');
        value = std::regex_replace(value, std::regex("kB"), "");
        memory_size = value;
      }
    }
  }
  long memory = stol(memory_size);
  memory /= 1024;
  return to_string(memory);
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string line, uid;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      string key, value;
      CleanString_(line, true);
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid") {
        uid = value;
      }
    }
  }
  return uid;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string line;
  string uid = LinuxParser::Uid(pid);
  std::ifstream  stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      CleanString_(line, true);
      string  key, placeholder, value;
      std::istringstream linestream(line);
      linestream >> key >> placeholder >> value;
      if (value == uid) {
        return key;
      }
    }
  }
  return string();
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    CleanString_(line, false);
    std::istringstream linestream(line);
    vector<string> values(std::istream_iterator<string>(linestream), {});
    float starttime = stof(values[21]) / sysconf(_SC_CLK_TCK);
    long system_uptime = LinuxParser::UpTime();
    return system_uptime - starttime;
  }
  return 0;
}

void  LinuxParser::CleanString_(string& line, bool add_underscore) {
  if (add_underscore) {
    std::replace(line.begin(), line.end(), ' ', '_');
  }
  std::replace(line.begin(), line.end(), '=', ' ');
  std::replace(line.begin(), line.end(), '"', ' ');
  std::replace(line.begin(), line.end(), ':', ' ');
}