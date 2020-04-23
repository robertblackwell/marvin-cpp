#!/bin/bash
# curl -H "Connection: close" -x localhost:9992 http://whiteacorn/utests/echo/index.php
project_dir=$(realpath `pwd`)
project_dir_basename=$(basename ${project_dir})
if [ "${project_dir_basename}" != "marvin++" ] ; then
	echo "You are in the wrong dir-should be marvin++"
	exit 
fi

cacerts=${project_dir}/.marvin/cert_store/root_certs/mozilla_ext.pem

# to verify the mozillat_ext actually has the mitm root certificate uncomment the next line and observer
# the curl request fail
# cacerts=${project_dir}/.marvin/cert_store/root_certs/mozilla.pem

insecure=

echo the project_dir: ${project_dir}
echo the cacerts: ${cacerts}

ls -al ${cacerts}

MARVIN_HOME= curl --cacert ${cacerts} ${insecure} --raw -x localhost:9992 https://geeksforgeeks.org
