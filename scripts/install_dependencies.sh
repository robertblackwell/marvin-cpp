# Install all depepdencies
debug=""
if [ "$1" == "help" ] ; then
	echo Install all dependencies boost, openssl, catch2, nlohmann_json
	echo Usage:
	echo 	install_dependencies.sh [arg]
	echo
	echo	args is either
	echo		help 	Print this help message
	echo		install After build copy include and libs to final destination
	echo
	echo 	The required packages are downloaded into a temp dir inside the scripts dir
	echo	If required the package is built and the headers and libs copied either
	echo 	to a temporary "stage" directory or to the final location		
	exit 0
fi

pwd=`pwd`
project_dir=$pwd
project_name=$(basename $project_dir)
source_dir=${project_dir}/marvin
external_src=${source_dir}/external_src
vendor=${project_dir}/vendor

if [ "$debug" != "" ] ; then echo $pwd; fi
if [ $project_name != "marvin++" ] ; then
	echo "You are in the wrong directory : [" ${project_name} "] should be at project root "
	exit 1
fi


if [ -d ${vendor} ] ; then
	if [ "$1" == "install"  ] ; then
		vendor_saved=${project_dir}/vendor-$(date +"%Y_%m_%d_%H_%M")
		echo ${vendor_saved}
		echo SAVING ${vendor} to ${vendor_saved}
		echo mv ${vendor} ${vendor_saved}
		mv ${vendor} ${vendor_saved}
		mkdir -p ${vendor}
	fi
else
	mkdir -p ${vendor}
fi

if [ -d ${external_src} ] ; then 
	if [ "$1" == "install" ] ; then
		rm -rf ${external_src}/*
	fi
else
	mkdir -p ${external_src}
fi

script_dir=$(dirname $(realpath $0))
clone_dir=${script_dir}/cloned_repos
stage_dir=${script_dir}/stage

if [ "$debug" != "" ] ; then echo should be external_code:  ${script_dir}; fi
if [ "$debug" != "" ] ; then echo should be external_code/cloned_repos:   ${clone_dir}; fi

if [ -d ${clone_dir} ] ; then 
	rm -rf ${clone_dir}/*
else
	mkdir -p ${clone_dir}
fi

if [ -d ${stage_dir} ] ; then 
	rm -rf ${stage_dir}/*
else
	mkdir -p ${stage_dir}
fi

${script_dir}/install_doctest.sh $1
${script_dir}/install_catch2.sh $1
${script_dir}/install_nlohmann_json_3.7.3.sh $1
# the following are external_src packages
${script_dir}/install_boost_1.72.0.sh $1
${script_dir}/install_openssl_1.1.1.sh $1
${script_dir}/install_http_parser.sh $1
${script_dir}/install_simple_buffer.sh $1
${script_dir}/install_urlparser.sh $1
${script_dir}/install_cxxurl.sh $1
${script_dir}/install_trog2.sh $1
${script_dir}/install_libcert.sh $1


if [ "$1" == "install"  ] || [ "$1" == "install_only" ] ; then
	${script_dir}/install_openssl_default_cert_file.sh
fi