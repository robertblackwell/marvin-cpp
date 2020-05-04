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

# ls -al ${cacerts}
comment=//
count=1
while IFS= read -r line
do
	if [[ $line == //* ]]
	then
		echo This is a comment line : $line
	else
		echo Server : ${count} ${line}
		MARVIN_HOME= curl --silent --show-error -L --cacert ${cacerts} ${insecure} --raw -x localhost:9992 https://${line} > /dev/null
		echo Server : ${count} ${line}
		let count=count+1
	fi
done < $1

# while IFS= read -r line; do echo $line ; done < data/list_top_500.data

# MARVIN_HOME= curl --cacert ${cacerts} ${insecure} --raw -x localhost:9992 https://hackernoon.com
