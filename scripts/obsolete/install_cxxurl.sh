#!/bin/bash
# https://github.com/json/json.git
debug=
cxxurl_release=v3.7.3
package_name=CxxUrl
git_repo=chmike/CxxUrl.git
git_branch=
clone_dir_stem_name=CxxUrl
header_cp_pattern=*.hpp
source_cp_pattern=url*.cpp


basedir=$(dirname "$0")
source ${basedir}/common_project.sh
source ${basedir}/common_external_src_pkg.sh
source ${basedir}/common_run.sh

