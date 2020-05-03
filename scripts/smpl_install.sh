# Install all depepdencies
debug=""

function smpl_install() {
	pwd=`pwd`
	project_dir=$pwd
	project_name=$(basename $project_dir)
	source_dir=${project_dir}/marvin
	external_src=${source_dir}/external_src
	vendor_dir=${project_dir}/vendor

	if [ "$debug" != "" ] ; then echo $pwd; fi
	if [ $project_name != "marvin++" ] ; then
		echo "You are in the wrong directory : [" ${project_name} "] should be at project root "
		exit 1
	fi


	if [ -d ${vendor_dir} ] ; then
		if [ "$1" == "install"  ] ; then
			vendor_saved=${project_dir}/vendor-$(date +"%Y_%m_%d_%H_%M")
			echo ${vendor_saved}
			echo SAVING ${vendor_dir} to ${vendor_saved}
			echo mv ${vendor_dir} ${vendor_saved}
			mv ${vendor_dir} ${vendor_saved}
			mkdir -p ${vendor_dir}
		fi
	else
		mkdir -p ${vendor_dir}
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

	function smpl_run_one() {
		if [ -f ${1} ]
		then
			package_script=${1}
			shift
			bash ${package_script} $@
		else
			echo WARNING: smpl_run_one ${package_script} does not exist
			exit 1
		fi
	}
	smpl_run_one ${script_dir}/doctest_2.3.7.sh $@
	smpl_run_one ${script_dir}/catch2_v2.11.1.sh $@
	smpl_run_one ${script_dir}/cli11_v1.9.0.sh $@
	smpl_run_one ${script_dir}/nlohmann_json_3.7.3.sh $@
	smpl_run_one ${script_dir}/boost_1.71.0.sh $@
	smpl_run_one ${script_dir}/openssl_1.1.1.sh $@
	smpl_run_one ${script_dir}/http_parser.sh $@
	smpl_run_one ${script_dir}/simple_buffer.sh $@
	smpl_run_one ${script_dir}/urlparser.sh $@
	smpl_run_one ${script_dir}/cxxurl.sh $@
	smpl_run_one ${script_dir}/trog.sh $@
	smpl_run_one ${script_dir}/libcert.sh $@

	if [ "$1" == "install"  ] || [ "$1" == "install_only" ] ; then
		${script_dir}/install_openssl_default_cert_file.sh
	fi
}
