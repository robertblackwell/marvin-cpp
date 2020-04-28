# runs a simple test mitm proxy to demonstrate capability
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

mitm_prog=${build_dir}/tests/test_proxy_manual/test_proxy_manual

# now run the proxy with the env variable MITM_HOME set to project root directory
MARVIN_HOME=${project_dir} valgrind --leak-check=yes --leak-resolution=low --undef-value-errors=no ${mitm_prog}