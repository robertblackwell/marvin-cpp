#!/bin/bash

debug=

boost_release=1.71.0
boost_dot_release=1.71.0
boost_underscore_release=1_71_0
boost_name=boost_1_71_0
boost_targz_file=boost_1_71_0
boost_url=https://dl.bintray.com/boostorg/release/${boost_release}/source/boost_1_71_0.tar.gz
boost_targz_file=boost_1_71_0.tar.gz

basedir=$(dirname "$0")
source ${basedir}/common_helpers.sh
source ${basedir}/common_project.sh
source ${basedir}/common_boost.sh
source ${basedir}/common_run.sh