#!/bin/bash
# https://github.com/json/json.git
debug=

package_name=trog
git_clone="git clone https://github.com/robertblackwell/trog"
clone_dir_stem_name=trog
header_cp_pattern=trog/*.hpp
source_cp_pattern=trog/*.cpp


basedir=$(dirname "$0")
source ${basedir}/inc_install_ext_pkg.sh

