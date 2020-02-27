#!/bin/bash
# https://github.com/openssl/openssl.git
#  This script installs a default certificate bundle at ${vendor}/ssl/cert.pem
# this is required because openssl_1.1.1d DOES NOT install such a file - older version did
# The location of this file is controller by the openssl build options --openssldir
# which in the accompanying install_openssl_vv_vv_vv.sh script is set to ${vendor}/ssl 
# 
#  The purpose of this script is to make sure that ${vendor}/ssl/cert.pem exists
#
# For fall back alternatives see include/cert_helpers.hpp and function Cert::Helpers::replace_openssl_get_default_cert_file

pwd=`pwd`

project_dir=$pwd
project_name=$(basename $project_dir)
vendor=${pwd}/vendor
mkdir -p ${vendor}/ssl

cp -v /usr/local/etc/openssl@1.1/cert.pem ${vendor}/ssl