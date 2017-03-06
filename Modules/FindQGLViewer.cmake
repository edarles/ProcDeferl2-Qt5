# - Find libQGLViewer	
# libQGLViewer is a C++ library based on Qt that eases
# the creation of OpenGL 3D viewers. 
# http://www.libqglviewer.com/
#
# This module defines the following variables	
#  QGLVIEWER_FOUND       - system has libQGLViewer
#  QGLVIEWER_INCLUDE_DIR - where the libQGLViewer include directory can be found
#  QGLVIEWER_LIBRARY     - link to this to use libQGLViewer
find_path(QGLVIEWER_INCLUDE_DIR QGLViewer/qglviewer.h)
find_library(QGLVIEWER_LIBRARY
             NAMES qglviewer-qt4 qglviewer QGLViewer QGLViewer2)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(libQGLViewer DEFAULT_MSG
                                  QGLVIEWER_LIBRARY QGLVIEWER_INCLUDE_DIR)
mark_as_advanced(QGLVIEWER_INCLUDE_DIR QGLVIEWER_LIBRARY)
