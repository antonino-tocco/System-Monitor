#include <iostream>
#include <vector>
#include <string>
#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
Processor::Processor() {
  std::vector<std::string> values = LinuxParser::CpuUtilization();
  user = stol(values[1]);
  nice = stol(values[2]);
  system = stol(values[3]);
  idle = stol(values[4]);
  iowait = stol(values[5]);
  irq = stol(values[6]);
  softirq = stol(values[7]);
  steal = stol(values[8]);
  guest = stol(values[9]);
  guest_nice = stol(values[10]);

  evaluated_idle = idle + iowait;
  not_idle = user + nice + system + softirq + steal;
  total = evaluated_idle + not_idle;
}
float Processor::Utilization() {
  return (float) (total - evaluated_idle) / total;
}