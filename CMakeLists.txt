cmake_minimum_required(VERSION 3.7)
project(smart_car)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "-lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_core")

include_directories(lane_detector)

add_executable(camera camera.cpp)
add_executable(joystick joystick.cpp)
add_executable(main main.cpp)

add_executable(lane_detector lane_detector/lane_detector.cpp)

add_library(controller controller.cpp)
add_library(ir_wall ir_wall_detector.cpp)
add_library(ir_line ir_line_detector.cpp)
add_library(binarization_utils lane_detector/binarization_utils.cpp)
add_library(bird_eye lane_detector/bird_eye.cpp)
add_library(line_utils lane_detector/line_utils.cpp)

link_directories(/usr/lib)
target_link_libraries(camera wiringPi crypt pthread rt m)
target_link_libraries(joystick wiringPi crypt pthread rt m controller)
target_link_libraries(main wiringPi crypt pthread rt m ir_wall ir_line)

link_directories(/usr/local/lib)
target_link_libraries(camera opencv_core opencv_highgui raspicam_cv)

find_package( OpenCV REQUIRED )
#include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(lane_detector binarization_utils bird_eye line_utils ${OpenCV_LIBS})