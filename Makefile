all: studio_daemon

studio_daemon: src/main.cpp src/daemon.cpp src/token
	g++ --std=c++0x -o studio_daemon src/main.cpp src/daemon.cpp -lwiringPi -lpthread

install: all script/launch_studio_daemon.sh
	install -m 0755 studio_daemon /usr/local/bin/studio_daemon
	install -m 0755 script/launch_studio_daemon.sh /etc/profile.d/launch_studio_daemon.sh	
