cmake_minimum_required(VERSION 3.12...3.16)

project(xmlrpc-task
  DESCRIPTION "AIStatefulTask submodule - XML RPC support"
  LANGUAGES CXX
)

include(AICxxProject)

#==============================================================================
# BUILD PROJECT
#

# This project is an OBJECT-library, used by other git submodules and the main project.
add_library(xmlrpc-task_ObjLib OBJECT)

# Set SOURCE_DIR to the current directory.
set_property(
  TARGET xmlrpc-task_ObjLib
  PROPERTY SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}"
)

# The list of source files.
target_sources(xmlrpc-task_ObjLib
  PRIVATE
    "XML_RPC_MethodCall.cxx"

    "XML_RPC_MethodCall.h"
)

# Required include search-paths.
get_target_property(CWDS_INTERFACE_INCLUDE_DIRECTORIES AICxx::cwds INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(xmlrpc-task_ObjLib
  PUBLIC
    "${CWDS_INTERFACE_INCLUDE_DIRECTORIES}" # For sys.h and debug.h.
)

# Require support for C++17.
target_compile_features(xmlrpc-task_ObjLib
  PUBLIC cxx_std_17
)

# Set link dependencies.
target_link_libraries(xmlrpc-task_ObjLib
  PUBLIC
    AICxx::socket-task
)

# Create an ALIAS target.
add_library(AICxx::xmlrpc-task ALIAS xmlrpc-task_ObjLib)
