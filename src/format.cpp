#include <iostream>
#include <string>
#include <ctime>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  char formatted_date[100];
  tm* elapsed_time;
  elapsed_time = localtime(&seconds);
  strftime(formatted_date, 50, "%d/%m/%Y %H:%M:%S", elapsed_time);
  return formatted_date;
}