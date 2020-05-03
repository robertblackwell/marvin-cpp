# Install all depepdencies
debug=""
source ./smpl_install

# function smpl_install() {
# 	pwd=`pwd`
# 	project_dir=$pwd
# 	project_name=$(basename $project_dir)
# 	source_dir=${project_dir}/marvin
# 	external_src=${source_dir}/external_src
# 	vendor_dir=${project_dir}/vendor

# 	if [ "$debug" != "" ] ; then echo $pwd; fi
# 	if [ $project_name != "marvin++" ] ; then
# 		echo "You are in the wrong directory : [" ${project_name} "] should be at project root "
# 		exit 1
# 	fi


# 	if [ -d ${vendor_dir} ] ; then
# 		if [ "$1" == "install"  ] ; then
# 			vendor_saved=${project_dir}/vendor-$(date +"%Y_%m_%d_%H_%M")
# 			echo ${vendor_saved}
# 			echo SAVING ${vendor} to ${vendor_saved}
# 			echo mv ${vendor} ${vendor_saved}
# 			mv ${vendor} ${vendor_saved}
# 			mkdir -p ${vendor}
# 		fi
# 	else
# 		mkdir -p ${vendor}
# 	fi

# 	if [ -d ${external_src} ] ; then 
# 		if [ "$1" == "install" ] ; then
# 			rm -rf ${external_src}/*
# 		fi
# 	else
# 		mkdir -p ${external_src}
# 	fi

# 	script_dir=$(dirname $(realpath $0))
# 	clone_dir=${script_dir}/cloned_repos
# 	stage_dir=${script_dir}/stage

# 	if [ "$debug" != "" ] ; then echo should be external_code:  ${script_dir}; fi
# 	if [ "$debug" != "" ] ; then echo should be external_code/cloned_repos:   ${clone_dir}; fi

# 	if [ -d ${clone_dir} ] ; then 
# 		rm -rf ${clone_dir}/*
# 	else
# 		mkdir -p ${clone_dir}
# 	fi

# 	if [ -d ${stage_dir} ] ; then 
# 		rm -rf ${stage_dir}/*
# 	else
# 		mkdir -p ${stage_dir}
# 	fi

# 	bash ${script_dir}/doctest_2.3.7.sh $1
# 	bash ${script_dir}/catch2_v2.11.1.sh $1
# 	bash ${script_dir}/nlohmann_json_3.7.3.sh $1
# 	bash ${script_dir}/boost_1.71.0.sh $1
# 	bash ${script_dir}/openssl_1.1.1.sh $1
# 	bash ${script_dir}/http_parser.sh $1
# 	bash ${script_dir}/simple_buffer.sh $1
# 	bash ${script_dir}/urlparser.sh $1
# 	bash ${script_dir}/cxxurl.sh $1
# 	bash ${script_dir}/trog.sh $1
# 	bash ${script_dir}/libcert.sh $1

# 	if [ "$1" == "install"  ] || [ "$1" == "install_only" ] ; then
# 		${script_dir}/install_openssl_default_cert_file.sh
# 	fi
# }

smpl_install $@