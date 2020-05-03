#!/bin/bash

debug=

boost_release=1.72.0
boost_dot_release=1.72.0
boost_underscore_release=1_72_0
boost_name=boost_1_72_0
boost_targz_file=boost_1_72_0
boost_url=https://dl.bintray.com/boostorg/release/${boost_release}/source/boost_1_72_0.tar.gz
boost_targz_file=boost_1_72_0.tar.gz

basedir=$(dirname "$0")
source ${basedir}/common_helpers.sh
source ${basedir}/common_project.sh
source ${basedir}/common_boost.sh
source ${basedir}/common_run.sh