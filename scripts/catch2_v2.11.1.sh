#!/bin/bash
# https://github.com/catch/catch.git

debug=

catch_release=v2.11.1
package_description=catch_v2.11.1
package_name=catch2

package_clone_stem=Catch2
git_clone="git clone https://github.com/catchorg/Catch2.git --branch ${catch_release}"
header_cp_pattern=single_include/catch2/*

basedir=$(dirname "$0")
source ${basedir}/common_helpers.sh
source ${basedir}/common_project.sh
source ${basedir}/common_header_only_pkg.sh
source ${basedir}/common_run.sh