cmake_minimum_required(VERSION 3.6)

project(bin2h)
set(CMAKE_CXX_STANDARD 11)

include(Dependency.cmake)

set(CMAKE_PROJECT_VERSION_MAJOR 0)
set(CMAKE_PROJECT_VERSION_MINOR 1)
set(CMAKE_PROJECT_VERSION_PATCH 0)

option(BIN2H_BUILD_TEST "build test" ON)
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/install" CACHE PATH "..." FORCE)
endif()

set(SOURCE_FILES
    src/util.h src/util.cpp)

add_executable(bin2h src/main.cpp ${SOURCE_FILES})
add_dependencies(bin2h ${DEP_PROJECTS})
target_include_directories(bin2h PUBLIC ${DEP_INSTALL_DIR}/include)
# target_link_libraries(bin2h PUBLIC -L${DEP_INSTALL_DIR}/lib spdlog)
target_compile_definitions(bin2h PRIVATE
    -DBIN2H_VERSION="${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR}.${CMAKE_PROJECT_VERSION_PATCH}")

message("-- bin2h install path: " "${CMAKE_INSTALL_PREFIX}/bin")
install(TARGETS bin2h DESTINATION bin)

if (${BIN2H_BUILD_TEST})
    enable_testing()
    add_executable(bin2h_test src/test.cpp ${SOURCE_FILES})
    add_dependencies(bin2h ${DEP_PROJECTS})
    target_include_directories(bin2h_test PUBLIC ${DEP_INSTALL_DIR}/include)

    add_test(NAME bin2h_test COMMAND bin2h_test)
endif()
