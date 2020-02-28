#!/bin/bash
# https://github.com/json/json.git
debug=
package_name=uri-parser
git_clone="git clone file://${HOME}/git-repos/uri-parser"
clone_dir_stem_name=uri-parser
header_cp_pattern=Uri*.hpp
source_cp_pattern=Uri*.cpp


basedir=$(dirname "$0")
source ${basedir}/inc_install_ext_pkg.sh

