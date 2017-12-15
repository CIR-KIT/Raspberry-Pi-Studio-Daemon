namespace
{

constexpr auto read_pin = 0;
constexpr auto token =
#include "token"
;

}

#include <cstdlib>
#include <sstream>

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

  auto last_value = digitalRead(read_pin);
  for (auto value = digitalRead(read_pin); true; last_value = value, value = digitalRead(read_pin)) {
    if (last_value == value)
      continue;

    if (value != LOW) {
      syslog(LOG_INFO, "Studio is open");
      notify(StudioState::open);
    } else {
      syslog(LOG_INFO, "Studio is closed");
      notify(StudioState::close);
    }
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
