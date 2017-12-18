#include <chrono>

namespace
{

constexpr auto read_pin = 0;
constexpr auto continually_time = std::chrono::seconds(10);
constexpr auto check_time = std::chrono::seconds(1);

constexpr const char* token[] = {
#include "token"
};

}

#include <cstdlib>
#include <sstream>
#include <thread>

extern "C" {
#include <syslog.h>
#include <wiringPi.h>
#include "run_daemon.h"
}

namespace
{

enum class StudioState
{
  open,
  close
};

void notify(const StudioState& state);

}

extern "C"
void run_daemon(const char* executable_name)
{
  openlog(executable_name, LOG_PID, LOG_DAEMON);
  if (wiringPiSetup()) {
    syslog(LOG_ERR, "Unavailable Wiring Pi");
    std::exit(EXIT_FAILURE);
  }
  pinMode(read_pin, INPUT);

  const auto continually_count = continually_time / check_time;
  auto last_value = digitalRead(read_pin);
  auto same_value_count = continually_count + 1;
  for (auto value = digitalRead(read_pin); true; last_value = value, value = digitalRead(read_pin)) {
    std::this_thread::sleep_for(check_time);

    if (last_value != value) {
      same_value_count = 0;
      continue;
    }

    if (same_value_count < continually_count) {
      ++same_value_count;
      continue;
    } else if (same_value_count == continually_count) {
      ++same_value_count; // onetime run this block
      if (value == LOW) {
        syslog(LOG_INFO, "Studio is open");
        notify(StudioState::open);
      } else {
        syslog(LOG_INFO, "Studio is closed");
        notify(StudioState::close);
      }
    } // ignore same value_count > continually_count
  }
}

namespace
{

void notify(const StudioState& state)
{
  for (auto it = std::begin(token), e = std::end(token); it != e; ++it) {
    std::ostringstream oss;
    oss << "curl -X POST -H 'Authorization: Bearer " << *it << "' -F 'message=";
    switch (state) {
    case StudioState::open:
      oss << "Studio is open.";
      break;
    case StudioState::close:
      oss << "Studio is closed.";
    }
    oss << "' https://notify-api.line.me/api/notify";
    syslog(LOG_INFO, "%d: %s", std::system(oss.str().c_str()), *it);
  }
}

}
