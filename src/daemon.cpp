#include <chrono>

namespace
{

constexpr auto read_pin {0};
constexpr auto continually_time {std::chrono::minutes{30}};
constexpr auto check_time {std::chrono::minutes{1}};

constexpr auto continually_count {continually_time / check_time};
constexpr auto token {
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
[[noreturn]]
void run_daemon(const char* executable_name)
{
  openlog(executable_name, LOG_PID, LOG_DAEMON);
  if (wiringPiSetup()) {
    syslog(LOG_ERR, "Unavailable Wiring Pi");
    std::exit(EXIT_FAILURE);
  }
  pinMode(read_pin, INPUT);

  auto last_value {digitalRead(read_pin)};
  auto same_value_count {0u};
  for (auto value {digitalRead(read_pin)}; true; last_value = value, value = digitalRead(read_pin)) {
    std::this_thread::sleep_for(check_time);

    if (last_value == value) {
      if (same_value_count < continually_count) {
        ++same_value_count;
        continue;
      }

      if (value == LOW) {
        syslog(LOG_INFO, "Studio is open");
        notify(StudioState::open);
      } else {
        syslog(LOG_INFO, "Studio is closed");
        notify(StudioState::close);
      }
    }

    same_value_count = 0;
  }
}

namespace
{

void notify(const StudioState& state)
{
  std::ostringstream oss;
  oss << "curl -X POST -H 'Authorization: Bearer " << token << "' -F 'message=";
  switch (state) {
  case StudioState::open:
    oss << "Studio is open.";
    break;
  case StudioState::close:
    oss << "Studio is closed.";
  }
  oss << "' https://notify-api.line.me/api/notify";
  static_cast<void>(std::system(oss.str().c_str()));
}

}
