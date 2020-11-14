cmake_minimum_required(VERSION 3.10)
# Enable C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_AUTOMOC TRUE)

# Define project name
project( ImageViewer )

find_package(OpenCV REQUIRED)
find_package(Qt5 COMPONENTS REQUIRED Core Gui Widgets)

# If the package has been found, several variables will
# be set, you can find the full list with descriptions
# in the OpenCVConfig.cmake file.
# Print some message showing some of them
message(STATUS "OpenCV library status:")
message(STATUS "config: ${OpenCV_DIR}")
message(STATUS "version: ${OpenCV_VERSION}")
message(STATUS "libraries: ${OpenCV_LIBS}")
message(STATUS "include path: ${OpenCV_INCLUDE_DIRS}")

include_directories(
    ${OpenCV_INCLUDE_DIRS}
)
link_directories(
    ${OpenCV_LIB_DIR}
)
# Declare the executable target built from your sources
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(project_sources
src/main.cc
src/mainwindow.cc
src/view.cc
src/image.cc
src/image_op.cc
src/util.cc
)
set(project_headers
include/mainwindow.h
include/view.h
include/image.h
include/image_op.h
include/util.h
)


add_executable(ImageViewer 
	${project_sources} 
	${project_headers}
)
target_include_directories(ImageViewer
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(ImageViewer 
	PUBLIC
	Qt5::Core
	Qt5::Gui 
	Qt5::Widgets
  ${OpenCV_LIBS})


# need this to include the auto generated ui_mainwindow.h 
# file from the qt5_wrap_ui call below.
set(CMAKE_INCLUDE_CURRENT_DIR ON)