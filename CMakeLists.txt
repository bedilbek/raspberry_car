cmake_minimum_required(VERSION 3.7)
project(smart_car)

set(CMAKE_CXX_STANDARD 14)

add_executable(camera camera.cpp)
add_executable(joystick joystick.cpp)
add_executable(main main.cpp)

add_library(controller controller.cpp)
add_library(ir_wall_detect ir_wall_detector.cpp)

link_directories(/usr/lib)
target_link_libraries(camera wiringPi crypt pthread rt m)
target_link_libraries(joystick wiringPi crypt pthread rt m controller)
target_link_libraries(main wiringPi crypt pthread rt m ir_wall_detect)

link_directories(/usr/local/lib)
target_link_libraries(camera opencv_core opencv_highgui raspicam_cv)
