# 
#  This file is intended to be included in other scripts not executed on its own
# 
#    Thats why no #! line at the top
# 
#  Contains common functions and code for installing full set of boost libraries
# 
#  The following variables must be set by the hosting script
# 
# boost_release=1.71.0
# boost_name=boost_1_71_0
# boost_targz_file=boost_1_71_0.tar.gz
# boost_url=https://dl.bintray.com/boostorg/release/${boost_release}/source/boost_1_71_0.tar.gz
# 
# A host scripts uses this script by "source" iing it after all of its own code
# 
#  This file requires these variables to be set
# requires
# 	project_name
# 	clone_dir
# 	stage_dir
#  	vendor_dir
# 	boost_name
# 	boost_targz_file
# 	boost_url
# 	boost_dir_name

function check_expected_varables() {
	w=-1
	for var in "$@"
	do
		if [ ${#var} -gt ${w} ]
		then
			w=${#var}
		fi
	done
	let w=w+1
	echo max length is : ${w}
	fmt="YYYYY  %-${w}s %s\n"
	echo fmt : ${fmt}
	filler=.............................
	for var in "$@" 
	do
		tmp=${var}${filler}
		tmp2=${tmp:0:w+5}
		printf "%-${w}s %s\n" ${tmp2} ${!var}
		if [ -z "${!var}" ] 
		then
			echo WARNING: variable ${var} is empty
			echo from function ${FUNCNAME[0]}
			echo called from line number : ${BASH_LINENO[1]} in file ${BASH_SOURCE[1]}
			echo terminating 
			exit 1
		fi
		# printf ${fmt} $var${filler:${#var}} ${!var}
		# printf "ZZZZZ  %-${w}s  %s\n" ${var} ${!var}
	done
}

# this will print the variables provided and terminate if any are 
# not defined
verify_print_variables \
	project_name \
	project_dir_name \
	clone_dir \
	stage_dir \
	vendor_dir \
	project_name \
	boost_targz_file \
	boost_url \
	rooster

exit 1


# [ -z "${project_name}" ] && echo WARNING[$ME]: variable project_name is empty  && exit 1
# [ -z "${project_dir_name}" ] && echo WARNING[$ME]: variable project_dir_name is empty  && exit 1
# [ -z "${clone_dir}" ] && echo WARNING[$ME]: variable clone_dir is empty   && exit 1
# [ -z "${stage_dir}" ] && echo WARNING[$ME]: variable stage_dir is empty   && exit 1
# [ -z "${vendor_dir}" ] && echo WARNING[$ME]: variable vendor_dir is empty  && exit 1
# [ -z "${boost_name}" ] && echo WARNING[$ME]: variable boost_name is empty  && exit 1
# [ -z "${boost_targz_file}" ] && echo WARNING[$ME]: variable boost_targz_file is empty  && exit 1
# [ -z "${boost_url}" ] && echo WARNING[$ME]: variable boost_url is empty  && exit 1

# echo project_name......................	${project_name}
# echo clone_dir.........................	${clone_dir}
# echo stage_dir.........................	${stage_dir}
# echo vendor_dir........................ ${vendor_dir}
# echo boost_name........................	${project_name}
# echo boost_targz_file..................	${boost_targz_file}
# echo boost_url.........................	${boost_url}

# 
# required 
# 	clone_dir
# 	boost_url
# 	boost_targz_file
# 	boost_name
# 
function get_package {
	cd ${clone_dir}
	rm -rfv ${clone_dir}/${boost_targz_file}*
	rm -rfv ${clone_dir}/${boost_name}
	wget ${boost_url}
	tar xvzf ${boost_targz_file}
	cd ${boost_name}
	ls -al
}

# required clone_dir
# 	stage_dir
# 	clone_dir
# 	boost_name
function stage_package {
	mkdir -p ${stage_dir}include
	mkdir -p ${stage_dir}/lib

	cd ${clone_dir}/${boost_name}

	./bootstrap.sh --prefix=${stage_dir}  darwin64-x86_64-cc

	./b2 --link=static --threading=multi --variant=debug --layout=system install
}

# required clone_dir
# 	project_name
# 	project_dir_name
# 
#  Obsolete project_common does this test
# 
function verify_package_name() {
	if [ $project_name != ${project_dir_name} ] ; then
		echo "You are in the wrong directory : [" ${project_name} "] should be at project root "
		exit 1
	fi
}
#  requires 
# 	vendor_dir
# 	stage_dir
function install_package {
	mkdir -p ${vendor_dir}/include/boost
	mkdir -p ${vendor_dir}/lib
	rm -rfv ${vendor_dir}/include/boost/*
	rm -rfv ${vendor_dir}/lib/libboost*
	cp -rv ${stage_dir}/include/boost/* ${vendor_dir}/include/boost

	cp -rv ${stage_dir}/lib/*.a ${vendor_dir}/lib/
	echo 
	echo INSTALL $boost_name complete ========================================================
	echo
}

function help() {
	echo Install package ${boost_name}
	echo Usage:
	echo 	stage_boost_x.xx.xx.sh [arg]
	echo
	echo	args is either
	echo		help 	Print this help message
	echo		install After build copy include and libs to final destination
	echo
	echo 	The required package is downloaded into a temp dir inside the scripts dir
	echo	If required the package is built and the headers and libs copied either
	echo 	to a temporary "stage" directory or to the final location		
	exit 0
}
