#!/bin/bash
# https://github.com/json/json.git
debug=

package_name=trog
git_clone="git clone https://github.com/robertblackwell/trog --branch=async"
package_clone_stem=trog
header_cp_pattern=include/trog/*.hpp

basedir=$(dirname "$0")
source ${basedir}/common_helpers.sh
source ${basedir}/common_project.sh
source ${basedir}/common_header_only_pkg.sh
source ${basedir}/common_run.sh
