#!/bin/bash

debug=

package_name=libcert
pwd=`pwd`
vendor=${pwd}/vendor
project_dir=$pwd
project_name=$(basename $project_dir)
script_dir=$(dirname $(realpath $0))
clone_dir=${script_dir}/cloned_repos
git_clone="git clone https://github.com/robertblackwell/x509_certificate_library.git ${release} ${clone_dir}/${package_name}"


basedir=$(dirname "$0")
source ${basedir}/common_helpers.sh
source ${basedir}/common_project.sh
source ${basedir}/common_libcert.sh
source ${basedir}/common_run.sh

