#!/bin/bash
# https://github.com/json/json.git
debug=

package_name=trog
git_clone="git clone https://github.com/robertblackwell/trog --branch=async"
package_clone_stem=trog
header_cp_pattern=include/trog/*.hpp

basedir=$(dirname "$0")
source ${basedir}/inc_install_hdr_pkg.sh

