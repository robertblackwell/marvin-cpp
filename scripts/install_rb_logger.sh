#!/bin/bash
# https://github.com/json/json.git
debug=

package_name=rb_logger
git_clone="git clone file://${HOME}/git-repos/rb_logger"
clone_dir_stem_name=rb_logger
header_cp_pattern=rb_logger/*.hpp
source_cp_pattern=rb_logger/*.cpp


basedir=$(dirname "$0")
source ${basedir}/inc_install_ext_pkg.sh

