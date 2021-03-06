#include <cstdlib>

extern "C" {
#include <unistd.h>
#include "run_daemon.h"
}

int main([[maybe_unused]] int argc, char** argv)
{
  if (daemon(0, 0))
    return EXIT_FAILURE;
  run_daemon(*argv);
}
