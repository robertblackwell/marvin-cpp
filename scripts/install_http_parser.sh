#!/bin/bash
# https://github.com/json/json.git
debug=
release=v2.9.3
package_name=http_parser
git_repo=nodejs/http-parser.git
git_branch=--branch ${release}
clone_dir_stem_name=http_parser
header_cp_pattern=http*.h
source_cp_pattern=http*.c

basedir=$(dirname "$0")
source ${basedir}/inc_install_ext_pkg.sh

