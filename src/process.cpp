#include <iostream>
#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <exception>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid): pid_(pid) {
  try {
    cpu_utilization = (float) LinuxParser::ActiveJiffies(pid) / LinuxParser::Jiffies();
  } catch (std::exception& e) {
    std::cout << "Exception " << e.what();
  }
}

// TODO: Return this process's ID
int Process::Pid() { return this->pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() {
  return cpu_utilization;
}

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(this->Pid()); }

// TODO: Return this process's memory utilization
string Process::Ram() {
    return LinuxParser::Ram(this->Pid());
}

// TODO: Return the user (name) that generated this process
string Process::User() {
  return LinuxParser::User(this->Pid());
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() {
  long uptime = 0;
  time_t current_time = 0;
  uptime = LinuxParser::UpTime(this->Pid());
  return current_time - uptime;
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a[[maybe_unused]]) const { return cpu_utilization < a.cpu_utilization; }