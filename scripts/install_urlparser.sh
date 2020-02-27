#!/bin/bash
# https://github.com/json/json.git
debug=
url_parser_release=v3.7.3
package_name=uri-parser
git_repo=CovenantEyes/uri-parser.git
git_branch=
clone_dir_stem_name=uri-parser
header_cp_pattern=Uri*.hpp
source_cp_pattern=Uri*.cpp

basedir=$(dirname "$0")
source ${basedir}/inc_install_ext_pkg.sh

