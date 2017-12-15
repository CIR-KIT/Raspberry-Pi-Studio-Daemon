cmake_minimum_required(VERSION 2.8)
project(StudioDaemon
  VERSION   0.1.0
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD          14  CACHE STRING "CXX standard version")
set(CMAKE_CXX_STANDARD_REQUIRED ON  CACHE BOOL   "version required")
set(CMAKE_CXX_EXTENSIONS        OFF CACHE BOOL   "compiler extension")

find_package(Threads REQUIRED)

add_executable(studio_daemon src/main.cpp src/daemon.cpp)
target_link_libraries(studio_daemon Threads::Threads wiringPi)

install(TARGETS studio_daemon
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
install(FILES script/launch_studio_daemon.sh
        DESTINATION /etc/profile.d
        PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ WORLD_READ)
