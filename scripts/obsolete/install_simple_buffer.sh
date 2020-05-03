#!/bin/bash
# https://github.com/json/json.git
debug=
simple_buffer_release=v3.7.3
package_name=simple_buffer
git_clone="git clone https://github.com/robertblackwell/simple_buffer"
clone_dir_stem_name=simple_buffer
header_cp_pattern=src/simple*.h
source_cp_pattern=src/simple*.c


basedir=$(dirname "$0")
source ${basedir}/common_project.sh
source ${basedir}/common_external_src_pkg.sh
source ${basedir}/common_run.sh

