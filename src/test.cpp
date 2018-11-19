#include <iostream>
#include <thread>
#include <chrono>

auto read_pin = 0; 

auto wait_time = std::chrono::seconds(1);

extern "C" {
#include <wiringPi.h>
}

int main(){
  if (wiringPiSetup() == -1) {
    std::cout << "error\n";
    return -1;
  }
  pinMode(read_pin, INPUT);
  while(true) {
    auto data = digitalRead(read_pin);
    if (data == HIGH) std::cout << "HIGH\n";
    else std::cout << "LOW\n";
    std::this_thread::sleep_for(wait_time);
  }
  return 0;
}
