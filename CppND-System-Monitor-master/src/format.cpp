#include <string>

#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
    int h = seconds / 3600;
    int m = (seconds / 60) % 60;
  	int s = seconds % 60;
    char buffer[10];
    sprintf(buffer, "%d:%02d:%02d", h, m, (int)s);
    string str = buffer;
    return str;
}