#!/bin/bash
# https://github.com/catch/catch.git

debug=

release=2.3.7
package_description=catch-mini
package_name=catch-mini

package_clone_stem=catch-mini
git_clone="git clone https://github.com/GValiente/catch-mini "
header_cp_pattern=catch.hpp


basedir=$(dirname "$0")
source ${basedir}/inc_install_hdr_pkg.sh

