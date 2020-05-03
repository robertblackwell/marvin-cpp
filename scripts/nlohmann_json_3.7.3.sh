#!/bin/bash
# https://github.com/json/json.git
debug=
project_name=marvin
json_release=v3=json
package_name=json
package_clone_stem=json
package_description=nlohman_json_${json_release}
git_clone="git clone https://github.com/nlohmann/json.git"
header_cp_pattern=single_include/nlohmann/json.hpp

basedir=$(dirname "$0")
source ${basedir}/common_helpers.sh
source ${basedir}/common_project.sh
source ${basedir}/common_header_only_pkg.sh
source ${basedir}/common_run.sh