#!/bin/bash
# https://github.com/json/json.git
debug=
package_name=uri-parser
# git_clone="git clone file://${HOME}/git-repos/uri-parser"
git_clone="git clone https://github.com/robertblackwell/urlparser"

clone_dir_stem_name=urlparser
header_cp_pattern=Uri*.hpp
source_cp_pattern=Uri*.cpp


basedir=$(dirname "$0")
source ${basedir}/common_helpers.sh
source ${basedir}/common_project.sh
source ${basedir}/common_external_src_pkg.sh
source ${basedir}/common_run.sh

