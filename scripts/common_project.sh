# 
# common variable setup for the marvin++ project
# 

# the basename of the project root directory and rel paths to the incude and src dirs that hold
#  project .cpp and .hpp files
project_dir_name=marvin++
project_src_dir_rel_path=marvin
project_include_dir_rel_path=marvin

#============================================================================================== 
# the following variables define where 3rd path include/lib and src iles will be installed
# 
#  step 1 is to define where the initial download of all 3rd party projects will be deposited
# 
#============================================================================================== 

# base name of the dir into which repos will be cloned
project_clone_dir_name=cloned_repos

#the path of the dir into which repos will be cloned - relative to the project dir
project_clone_dir_rel_path=scripts/${project_clone_dir_name}

#============================================================================================== 
# 
# step 2 of getting 3rd party projects is to "build" and install them insall
# them into a staging area - thats what we now define
# 
#============================================================================================== 

# base name of the dir into which repos will be staged
project_stage_dir_name=stage

#the path of the dir into which repos will be cloned - relative to the project dir
project_stage_dir_rel_path=scripts/${project_stage_dir_name}

#============================================================================================== 
# 
# step 3 of getting 3rd party projects is to copy the required files to their final 
#  location - we now define that location - it is termed the vendor dir
# 
#============================================================================================== 
# the base name of the dir that will hold the installed include/ lib/ src/
project_vendor_dir_base_name=vendor

# path to vendor dir relative to project dir
project_vendor_dir_rel_path=vendor

project_vendor_include_dir_rel_path=vendor/include
project_vendor_lib_dir_rel_path=vendor/lib 
project_vendor_src_dir_rel_path=vendor/src 

#============================================================================================== 
# 
#  now derive full paths for the from veriables
# 
#============================================================================================== 

pwd=`pwd`
current_dir_base_name=$(basename `pwd`)
if [ ${current_dir_base_name} != ${project_dir_name} ]
then
	echo ERROR - these scripts must be run from the project root dircetory
	echo project root directory name is : ${project_dir_name}
	echo current directory is           : ${pwd}
	echo ===========================================================================
	exit 1
fi

project_dir=$pwd
project_name=$(basename $project_dir)
source_dir=${project_dir}/${project_src_dir_rel_path}
include_dir=${project_dir}/${project_src_dir_rel_path}
# external_src=${source_dir}/external_src
external_src=${project_dir}/${project_vendor_src_dir_rel_path}

clone_dir=${project_dir}/${project_clone_dir_rel_path}

stage_dir=${project_dir}/${project_stage_dir_rel_path}
stage_include_dir=${stage_dir}/include
stage_lib_dir=${stage_dir}/lib
stage_src_dir=${stage_dir}/src

vendor=./dontuse/directoru
vendor_dir=${project_dir}/${project_vendor_dir_base_name}
vendor_include_dir=${project_dir}/${project_vendor_include_dir_rel_path}
vendor_lib_dir=${project_dir}/${project_vendor_lib_dir_rel_path}
vendor_src_dir=${project_dir}/${project_vendor_src_dir_rel_path}

package_stage_dir=${stage_src_dir}/{package_name}
}
 
# echo project_dir_name......................	${project_dir_name}
# echo project_src_dir_rel_path..............	${project_src_dir_rel_path}
# echo project_include_dir_rel_path..........	$project_include_dir_rel_path}
# echo project_clone_dir_name................	${project_clone_dir_name}
# echo project_clone_dir_rel_path............	${project_clone_dir_rel_path}
# echo project_stage_dir_name................	${project_stage_dir_name}
# echo project_stage_dir_rel_path............	${project_stage_dir_rel_path}
# echo project_vendor_dir_base_name..........	${project_vendor_dir_base_name}
# echo project_vendor_dir_rel_path...........	${project_vendor_dir_rel_path}
# echo project_vendor_include_dir_rel_path... ${project_vendor_include_dir_rel_path}
# echo project_vendor_lib_dir_rel_path.......	${project_vendor_lib_dir_rel_path} 
# echo project_vendor_src_dir_rel_path.......	${project_vendor_src_dir_rel_path} 

# echo project_dir........................... $project_dir
# echo project_name.......................... $project_name
# echo vendor_dir............................ $vendor_dir
# echo source_dir............................ $source_dir
# echo include_dir........................... $include_dir
# echo external_src.......................... $external_src
# echo clone_dir............................. $clone_dir
# echo stage_dir............................. $stage_dir
# echo vendor................................ $vendor
# echo vendor_dir............................ $vendor_dir
# echo vendor_include_dir.................... $vendor_include_dir
# echo vendor_lib_dir........................ $vendor_lib_dir
# echo vendor_src_dir........................ $vendor_src_dir

verify_print_variables \
	project_dir_name  \
	project_src_dir_rel_path  \
	project_include_dir_rel_path  \
	project_clone_dir_name  \
	project_clone_dir_rel_path  \
	project_stage_dir_name  \
	project_stage_dir_rel_path  \
	project_vendor_dir_base_name  \
	project_vendor_dir_rel_path  \
	project_vendor_include_dir_rel_path  \
	project_vendor_lib_dir_rel_path  \
	project_vendor_src_dir_rel_path  \
	project_dir  \
	project_name  \
	vendor_dir  \
	source_dir  \
	include_dir  \
	external_src  \
	clone_dir  \
	stage_dir  \
	vendor  \
	vendor_dir  \
	vendor_include_dir  \
	vendor_lib_dir  \
	vendor_src_dir
