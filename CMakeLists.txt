cmake_minimum_required(VERSION 3.16)

##
## NOTE/WARNING - this file will not work if trying to build anywhere other 
## than inside the marvin++ repo
##
## That is - knows nothing about installing this project
##

##
## Project 
## name  version
##
project(Marvin VERSION 0.1.0 LANGUAGES CXX C)

##
## Includes
##
##

##
## find packages
##
##
find_package(Threads REQUIRED)
find_program(BASH_PROG bash)
message("BASH_PROG-NOTFOUND       ${BASH_PROG-NOTFOUND}")
message("BASH_PROG                ${BASH_PROG}")
## not sure any of these work for the moment so turn them OFF
if(OFF)
find_package(boost)
find_package(openssl)
find_package(trog)
find_package(x509_certificate_library)
endif()

##
## options
##
option(MARVIN_BuildTests "Build the tests when enabled" ON )
option(MARVIN_Install_Targets "Installs targets as well as files if set" OFF)
option(MARVIN_Package "Install as a CMake package - not implemented" OFF)
option(MARVIN_Verbose "Print a lot of diagnostic stuff" ON)
option(MARVIN_DebugBuild "Perform build as type Debug" ON)



## ============================================================================
## The stuff below - I am not sure about ======================================
##

# this is a fix for a problem with boost libraries see https://github.com/Microsoft/vcpkg/issues/4497
# also a hack and breaks "best practices" as per Damiel Pfeiffer's presentation https://www.youtube.com/watch?v=rLopVhns4Zs&feature=youtu.be
set(other_flags "${other_flags} -frtti -fvisibility-inlines-hidden")
set(other_flags "${other_flags} -fvisibility=hidden")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${other_flags} -pthread")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
set(CTEST_CUSTOM_PRE_TEST "echo THIS IS PRE TEST")
set(THREADS_PREFER_PTHREAD_FLAG ON)

# Let's nicely support folders in IDE's
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
# allow scheme porperties in xcode - particularly environment variables
set_property(GLOBAL PROPERTY XCODE_GENERATE_SCHEME ON)
set(XCODE_GENERATE_SCHEME ON)
##
## The stuff below - I am not sure about ====================================
## ==========================================================================

##
##
## Configuration
##
include (GNUInstallDirs)

if (MARVIN_DebugBuild)
	set(CMAKE_BUILD_TYPE Debug)
endif()
set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)
if(DEFINED CMAKE_DL_LIBS)
    set(MARVIN_DL_LIB_SO libdl.so)
endif()

set(MARVIN_PROJECT_DIR     ${Marvin_SOURCE_DIR})
## location of project header and source files
set(MARVIN_SOURCE_NAME      marvin)
set(MARVIN_INCLUDE_NAME     marvin)
set(MARVIN_SOURCE_DIR       ${MARVIN_PROJECT_DIR}/${MARVIN_SOURCE_NAME})
set(MARVIN_INCLUDE_DIR      ${MARVIN_PROJECT_DIR}/${MARVIN_INCLUDE_NAME})
## location of other/3rd party headers, src and libs
set(MARVIN_VENDOR_NAME		vendor)
set(MARVIN_VENDOR_DIR       ${MARVIN_PROJECT_DIR}/${MARVIN_VENDOR_NAME})
set(MARVIN_VENDOR_INCLUDEDIR ${MARVIN_VENDOR_DIR}/include)
set(MARVIN_VENDOR_LIBDIR    ${MARVIN_VENDOR_DIR}/lib)
set(MARVIN_VENDOR_SRCDIR    ${MARVIN_VENDOR_DIR}/src)

## used by all compile steps to find headers
set(MARVIN_INCLUDE_PATHS 
	${MARVIN_PROJECT_DIR}
	# ${MARVIN_INCLUDE_DIR}
	# ${MARVIN_SOURCE_DIR}
	${MARVIN_VENDOR_INCLUDEDIR}
	${MARVIN_VENDOR_SRCDIR}
)
set(MARVIN_LINK_LIBRARIES 
	marvin_library 
	Threads::Threads 
	${CMAKE_DL_LIBS}
	${MARVIN_VENDOR_LIBDIR}/libcert_library.a
	${MARVIN_VENDOR_LIBDIR}/libboost_filesystem.a 
	${MARVIN_VENDOR_LIBDIR}/libboost_system.a
	${MARVIN_VENDOR_LIBDIR}/libboost_program_options.a
	${MARVIN_VENDOR_LIBDIR}/libssl.a 
	${MARVIN_VENDOR_LIBDIR}/libcrypto.a 
	${CMAKE_DL_LIBS}
	${DL_LIB_SO}
	)


set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY XCODE_GENERATE_SCHEME ON)

if (MARVIN_Verbose)
	message("Project Name                      ${CMAKE_PROJECT_NAME}")
	message("Build Type                        ${CMAKE_BUILD_TYPE}")
	message("CMAKE_CXX_STANDARD                ${CMAKE_CXX_STANDARD}")
	message("CMAKE_DL_LIBS                     ${CMAKE_DL_LIBS}")
	message("MARVIN_PROJECT_DIR                ${MARVIN_PROJECT_DIR}")
	message("MARVIN_SOURCE_DIR                 ${MARVIN_SOURCE_DIR}")
	message("MARVIN_INCLUDE_DIR                ${MARVIN_INCLUDE_DIR}")
	message("MARVIN_VENDOR_NAME                ${MARVIN_VENDOR_NAME}")
	message("MARVIN_VENOR_DIR                  ${MARVIN_VENDOR_DIR}")
	message("MARVIN_VENDOR_INCLUDEDIR          ${MARVIN_VENDOR_INCLUDEDIR}")
	message("MARVIN_VENDOR_LIBDIR              ${MARVIN_VENDOR_LIBDIR}")
	message("MARVIN_VENDOR_SRCDIR              ${MARVIN_VENDOR_SRCDIR}")
	message("MARVIN_VENDOR_SRCDIR              ${MARVIN_VENDOR_SRCDIR}")
	message("MARVIN_DL_LIB_SO                  ${MARVIN_DL_LIB_SO}")
	message("MARVIN_INCLUDE_PATHS              ${MARVIN_INCLUDE_PATHS}")
	message("MARVIN_LINK_LIBRARIES             ${MARVIN_LINK_LIBRARIES}")
	message("CMAKE_INSTALL_PREFIX              ${CMAKE_INSTALL_PREFIX}")
	message("CMAKE_INSTALL_FULL_INCLUDEDIR     ${CMAKE_INSTALL_FULL_INCLUDEDIR}")
	message("CMAKE_INSTALL_INCLUDEDIR          ${CMAKE_INSTALL_INCLUDEDIR}")
endif()

##
## Enabled testing
##
include(CTest) 
enable_testing()

add_subdirectory(marvin)

add_subdirectory(apps)
add_subdirectory(tests)


# set(CMAKE_INSTALL_PREFIX ${PROJECT_DIR})
