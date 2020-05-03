#!/bin/bash
# https://github.com/openssl/openssl.git
pwd=`pwd`
project_dir=$pwd
project_name=$(basename $project_dir)
vendor=${pwd}/vendor
script_dir=$(dirname $(realpath $0))
clone_dir=${script_dir}/cloned_repos
openssl_name=openssl-1.1.1f

basedir=$(dirname "$0")
source ${basedir}/common_helpers.sh
source ${basedir}/common_project.sh
source ${basedir}/common_openssl.sh
source ${basedir}/common_run.sh
