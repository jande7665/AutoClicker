cmake_minimum_required(VERSION 3.5)
project(AutoClicker)

# Set the C++ standard to C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})

add_executable(AutoClicker main.cpp MainWindow.cpp)
target_link_libraries(AutoClicker ${wxWidgets_LIBRARIES})

if (APPLE)
    target_link_libraries(AutoClicker "-framework CoreGraphics")
endif()
