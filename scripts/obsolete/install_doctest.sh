#!/bin/bash
# https://github.com/catch/catch.git

debug=

release=2.3.7
package_description=doctest_v2.3.7
package_name=doctest

package_clone_stem=doctest
git_clone="git clone https://github.com/onqtam/doctest.git --branch ${release}"
header_cp_pattern=doctest/doctest.h


basedir=$(dirname "$0")
source ${basedir}/common_project.sh
source ${basedir}/common_header_only_pkg.sh
source ${basedir}/common_run.sh