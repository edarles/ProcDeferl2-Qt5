cmake_minimum_required(VERSION 2.8.3)
project(ProcDeferl)

set(CMAKE_C_COMPILER "/usr/bin/g++")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")
set(CMAKE_CC_COMPILER "/usr/bin/g++")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -W") 
SET(CMAKE_CC_FLAGS "${CMAKE_CC_FLAGS} -std=c++11 -O3 -W") 

SET(CMAKE_VERBOSE_MAKEFILE OFF) 
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(GLUT REQUIRED)
FIND_PACKAGE(GLEW REQUIRED)
FIND_PACKAGE(Eigen3 REQUIRED)
FIND_PACKAGE(QGLViewer REQUIRED)
FIND_PACKAGE(TinyXML REQUIRED)


include_directories( ${GLEW_INCLUDE_DIR} ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS})
include_directories( ${EIGEN3_INCLUDE_DIR})
include_directories( ${QGLVIEWER_INCLUDE_DIR})
include_directories( ${TINYXML_INCLUDE_DIR})

# Compilation et execution avec QT5
find_package(Qt5Widgets)
find_package(Qt5Core)
find_package(Qt5Gui)
find_package(Qt5OpenGL)
find_package(Qt5Xml)
add_definitions(${Qt5Widgets_DEFINITIONS})
include_directories(${Qt5Widgets_INCLUDE_DIRS} ${Qt5Xml_INCLUDE_DIRS} ${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${Qt5Xml_INCLUDE_DIRS})

include(${CMAKE_CURRENT_SOURCE_DIR}/core/CMakeLists.txt)

ADD_EXECUTABLE(ProcDeferl2 ${APP_SOURCES})

TARGET_LINK_LIBRARIES(ProcDeferl2 -lstdc++ ${QGLVIEWER_LIBRARY} ${GLEW_LIBRARY} ${OPENGL_LIBRARY} ${GLUT_glut_LIBRARY} ${EIGEN3_LIBRARY}
	${Qt5Widgets_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5Core_LIBRARIES} ${Qt5Gui_LIBRARIES} ${Qt5OpenGL_LIBRARIES} ${TINYXML_LIBRARIES})
