cmake_minimum_required(VERSION 3.7)
project(smart_car)

set(CMAKE_CXX_STANDARD 14)

add_executable(smart_car main.cpp)
link_directories(/usr/lib)
target_link_libraries(smart_car wiringPi crypt pthread rt m)
link_directories(/usr/local/lib)
target_link_libraries(smart_car opencv_core opencv_highgui raspicam_cv)
