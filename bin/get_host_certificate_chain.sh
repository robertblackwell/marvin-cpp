#!/bin/bash
# curl -H "Connection: close" -x localhost:9992 http://whiteacorn/utests/echo/index.php
project_dir=$(realpath `pwd`)
project_dir_basename=$(basename ${project_dir})
if [ "${project_dir_basename}" != "marvin++" ] ; then
	echo "You are in the wrong dir-should be marvin++"
	exit 
fi

cacerts=${project_dir}/.marvin/cert_store/root_certs/mozilla_ext.pem

echo | openssl s_client -showcerts -CAfile ${cacerts} -servername $1 -connect $1:443 2>/dev/null | openssl x509 -inform pem -noout -text