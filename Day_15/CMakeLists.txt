﻿cmake_minimum_required (VERSION 3.8)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Day_15")

set(EXTERNAL_DIR "../utils")
file(GLOB EXTERNAL_SOURCES "${EXTERNAL_DIR}/*.cpp" "${EXTERNAL_DIR}/*.hpp")
add_executable (Day_15 "Day_15.cpp" ${EXTERNAL_SOURCES})
target_include_directories(Day_15 PRIVATE ${EXTERNAL_DIR})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Day_15 PROPERTY CXX_STANDARD 20)
endif()
