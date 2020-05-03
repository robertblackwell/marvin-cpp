#!/bin/bash
# https://github.com/catch/catch.git

debug=

release=v1.9.0
package_description=CLI11_v2.11.1
package_name=CLI

package_clone_stem=CLI11
git_clone="git clone https://github.com/CLIUtils/CLI11.git --branch ${release}"
header_cp_pattern=include/CLI/*

basedir=$(dirname "$0")
source ${basedir}/common_project.sh
source ${basedir}/common_header_only_pkg.sh
source ${basedir}/common_run.sh