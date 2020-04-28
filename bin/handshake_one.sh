
project_dir=$(realpath `pwd`)
project_dir_basename=$(basename ${project_dir})
if [ "${project_dir_basename}" != "marvin++" ] ; then
	echo "You are in the wrong dir-should be marvin++"
	exit 
fi

echo the project_dir: ${project_dir}

if [[ $1 != "" ]]
then 
	marvin_arg=--marvin
else
	marvin_arg=
fi

MARVIN_HOME=${project_dir} ${project_dir}/cmake-build-debug/apps/handshake/handshake ${marvin_arg} --host ${1}