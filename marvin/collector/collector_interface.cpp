//
// The main entry point for Marvin - a mitm proxy for http/https 
//
#include <marvin/collector/collector_interface.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include <marvin/boost_stuff.hpp>
#include <marvin/external_src/trog/trog.hpp>
Trog_SETLEVEL(LOG_LEVEL_WARN)


