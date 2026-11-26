// Copyright 2015 The Crashpad Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <time.h>

extern "C" {

struct tm* gmtime_r(const time_t* timep, struct tm* result) {
  if (gmtime_s(result, timep) != 0)
    return nullptr;
  return result;
}

struct tm* localtime_r(const time_t* timep, struct tm* result) {
  if (localtime_s(result, timep) != 0)
    return nullptr;
  return result;
}

const char* strptime(const char* buf, const char* format, struct tm* tm) {
  // TODO(scottmg): strptime implementation.
  return nullptr;
}

#ifdef _MSC_VER
time_t timegm(struct tm* tm) {
  return _mkgmtime(tm);
}
#else
// https://stackoverflow.com/questions/16647819/timegm-cross-platform
// Algorithm: http://howardhinnant.github.io/date_algorithms.html
int days_from_epoch(int y, int m, int d) {
    y -= m <= 2;
    int era = y / 400;
    int yoe = y - era * 400;                                   // [0, 399]
    int doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;  // [0, 365]
    int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;           // [0, 146096]
    return era * 146097 + doe - 719468;
}

// It  does not modify broken-down time
time_t timegm(struct tm* tm) {
    int year = tm->tm_year + 1900;
    int month = tm->tm_mon;          // 0-11
    if (month > 11) {
        year += month / 12;
        month %= 12;
    } else if (month < 0) {
        int years_diff = (11 - month) / 12;
        year -= years_diff;
        month += 12 * years_diff;
    }
    int days_since_epoch = days_from_epoch(year, month + 1, tm->tm_mday);

    return 60 * (60 * (24L * days_since_epoch + tm->tm_hour) + tm->tm_min) + tm->tm_sec;
}
#endif

}  // extern "C"
