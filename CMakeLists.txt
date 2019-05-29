cmake_minimum_required(VERSION 2.8)
PROJECT(RdpInfo)

set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        )
foreach(CompilerFlag ${CompilerFlags})
  string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
endforeach()

add_compile_options(/source-charset:utf-8)
add_definitions(-DUNICODE -D_CRT_SECURE_NO_WARNINGS)

add_library(${PROJECT_NAME} SHARED
    src/dllmain.cpp
    src/stdafx.cpp
    src/RdpInfo.cpp
    src/RdpInfo.def
    src/TerminalServicesSession.cpp
    src/Wexception.cpp
)

target_link_libraries(${PROJECT_NAME} ws2_32 Wtsapi32)
