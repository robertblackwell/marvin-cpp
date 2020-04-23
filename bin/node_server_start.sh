# runs a simple test mitm proxy to demonstrate capability
project_dir=$(realpath `pwd`)
project_dir_basename=$(basename ${project_dir})
if [ "${project_dir_basename}" != "marvin++" ] ; then
	echo "You are in the wrong dir-should be marvin++"
	exit 
fi
test_dir=${project_dir}/tests

if [ ! -d ${build_dir} ] ; then
	echo build directory ${build_dir} does not exist
	exit
fi 

node_server=${project_dir}/tests/tools/js_server/server.js

# now run the proxy with the env variable MITM_HOME set to project root directory
node ${node_server}