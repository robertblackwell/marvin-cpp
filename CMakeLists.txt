# Works with 3.11 and tested through 3.15
cmake_minimum_required(VERSION 3.11...3.16)

set(THISFILE_DEBUG 0)

# Project name and a few useful settings. Other commands can pick up the results
project(Marvin
    VERSION 0.1
    DESCRIPTION "A man in the middle proxy"
    LANGUAGES CXX)


# this is a fix for a problem with boost libraries see https://github.com/Microsoft/vcpkg/issues/4497
# also a hack and breaks "best practices" as per Damiel Pfeiffer's presentation https://www.youtube.com/watch?v=rLopVhns4Zs&feature=youtu.be
set(other_flags "${other_flags} -frtti -fvisibility-inlines-hidden")
set(other_flags "${other_flags} -fvisibility=hidden")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${other_flags}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_program(iwyu_path NAMES include-what-you-use iwyu)
if(NOT iwyu_path)
  message(FATAL_ERROR "Could not find the program include-what-you-use")
endif()

    MESSAGE("INSIDE IF STATEMENT")
    # Optionally set things like CMAKE_CXX_STANDARD, CMAKE_POSITION_INDEPENDENT_CODE here
    # Let's ensure -std=c++xx instead of -std=g++xx
    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)

    # Let's nicely support folders in IDE's
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    # allow scheme porperties in xcode - particularly environment variables
    set_property(GLOBAL PROPERTY XCODE_GENERATE_SCHEME ON)
    set(XCODE_GENERATE_SCHEME ON)
    include(CTest) 
    enable_testing()

set(PROJECT_DIR ${Marvin_SOURCE_DIR})
set(VENDOR_DIR ${PROJECT_DIR}/vendor)

message(Module Path : ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} /usr/local/Cellar/cmake3.16.3/share/cmake/Modules)
set(APPS_LINK_LIBS marvin_library)
# add_subdirectory(apps)
add_subdirectory(marvin)
add_subdirectory(tests)


set(CMAKE_INSTALL_PREFIX ${PROJECT_DIR})
