cmake_minimum_required(VERSION 3.7)
project(smart_car)

set(CMAKE_CXX_STANDARD 17)


include_directories(lane_detector)

add_custom_target(smart_car ALL)

IF(WIN32)
    SET(OpenCV_DIR "C:\\opencv\\mingw-build\\install")
ENDIF()

find_package( OpenCV REQUIRED )

IF(WIN32)
    include_directories(${OpenCV_INCLUDE_DIRS})
ENDIF()

add_executable(camera camera.cpp)
add_executable(joystick joystick.cpp)
add_executable(main main.cpp)
add_executable(zebra zebra.cpp)
add_executable(term terminal.cpp)

add_executable(lane_detector lane_detector/lane_detector.cpp)
add_executable(lane_detector_img lane_detector/lane_detector_img.cpp)
add_executable(traffic_light_detection traffic_light_detection/traffic_light_detector.cpp)

add_library(controller controller.cpp)
add_library(ir_wall ir_wall_detector.cpp)
add_library(ir_line ir_line_detector.cpp)
add_library(cross_walk_detector cross_walk_detector.cpp)

# Lane Detection Libs
add_library(binarization_utils lane_detector/binarization_utils.cpp color_masking.cpp)
add_library(bird_eye lane_detector/bird_eye.cpp)
add_library(line_utils lane_detector/line_utils.cpp)
add_library(poly_regres lane_detector/poly_fit.cpp)

# Linking Libs from /usr/lib directory
link_directories(/usr/lib)
target_link_libraries(camera wiringPi crypt pthread rt m)
target_link_libraries(joystick wiringPi crypt pthread rt m controller)
target_link_libraries(main wiringPi crypt pthread rt m ir_wall ir_line controller)

# Linking Libs from /usr/local/lib directory
link_directories(/usr/local/lib)
target_link_libraries(lane_detector binarization_utils bird_eye line_utils poly_regres ${OpenCV_LIBS})
target_link_libraries(traffic_light_detection ${OpenCV_LIBS})
target_link_libraries(lane_detector controller wiringPi raspicam_cv binarization_utils bird_eye line_utils poly_regres ${OpenCV_LIBS})
target_link_libraries(lane_detector_img binarization_utils bird_eye line_utils poly_regres ${OpenCV_LIBS})
target_link_libraries(camera raspicam_cv ${OpenCV_LIBS})
target_link_libraries(zebra bird_eye binarization_utils ${OpenCV_LIBS})
target_link_libraries(main raspicam_cv ${OpenCV_LIBS})

target_link_libraries(term ncurses pthread ${OpenCV_LIBS})

set (source "${CMAKE_SOURCE_DIR}/files")
set (destination "${CMAKE_CURRENT_BINARY_DIR}/files")

#install(DIRECTORY files/ DESTINATION shakhob/debil)
add_custom_command(
        TARGET lane_detector POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${source}/ ${destination}
        DEPENDS ${destination}
        COMMENT "symbolic link resources folder from ${source} => ${destination}"
)