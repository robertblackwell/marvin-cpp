# this script should be run before running ctest in the build directory
# it runs a simple node server on localhost:3000 which some of the tests communicate with
project_dir=$(realpath `pwd`)
project_dir_basename=$(basename ${project_dir})
if [ "${project_dir_basename}" != "marvin++" ] ; then
	echo "You are in the wrong dir-should be marvin++"
	exit 
fi
build_dir=${project_dir}/cmake-build-debug

if [ ! -d ${build_dir} ] ; then
	echo build directory ${build_dir} does not exist
	exit
fi 
node_server=${project_dir}/tests/tools/js_server/server_01/server.js
if [ ! -f ${node_server} ] ; then 
	echo The node server does not exist
	exit
fi

node ${node_server}

