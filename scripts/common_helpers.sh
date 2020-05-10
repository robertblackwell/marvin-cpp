# usage
# check_expected_varables \
#   project_name \
#   project_dir_name \
#   clone_dir \
#   stage_dir \
#   vendor_dir \
#   project_name \
#   boost_targz_file \
#   boost_url \
#   rooster

# 
# prints a block of text didplaying the names and values of a list of variables
#  
# 
function smpl_help {
cat <<SMPL_HELP_TEXT_HDOC
smpl - is a sh version of smpl (there is a py version also) a very elementary dependency installer.
 
It will download, build other projects and copy necessary files from the build dir to 
where you want them for use in your project.

Usage:
    smpl cmd [arg]
    cmd can be one of 
        
        install without an argument will download build and install all
                the 3rd party packages you have defined to smpl.
        install arg
                with an argument. The argument is a name of one of the packages you have
                already defined to smpl. If not you will get an error.
                That one package will be downloaded built and installed.
        clean   without an argument all installed files and all artifacts from
                all previous installs will be deleted
        clean   arg
                you guessed it - will clean all installed files and all artifacts from
                a previous install of the named package.
        install_only 
        install_only arg
                starts from a previous download and build, and only does the "copy" from
                the staging locaton.
    smpl does the following:
    -   downloads either via git clone or, wget of a tar.gz file, into a "clone_dir" 
        the location of which can be configured.
    -   in the downloaded repo each package is built and installed to a staging location
        called "stage_dir". The packages own build and install features are used.
    -   relevant header, source, library and what-ever files are copied from the staging
        location to place where your project needs them. This is called "vendor_dir"
        and that is also configurable.
SMPL_HELP_TEXT_HDOC
    exit 0
}

function print_varables() {
    local w=-1
    for var in "$@"
    do
        if [ ${#var} -gt ${w} ]
        then
            w=${#var}
        fi
    done
    let w=w+1
    # echo max length is : ${w}
    fmt="YYYYY  %-${w}s %s\n"
    # echo fmt : ${fmt}
    filler=........................................................................................
    for var in "$@" 
    do
        tmp=${var}${filler}
        tmp2=${tmp:0:w+5}
        printf "%-${w}s %s\n" ${tmp2} ${!var}

    done
}
function source_trace() {

    n=${#BASH_SOURCE[@]}
    for (( i=0; i<${n}; i++ ));
    do
      echo from ${BASH_LINENO[$i]} in file ${BASH_SOURCE[$i+1]}
    done
}
# 
# checks that each variable in the input list is defined
# terminates script if not
# 
function verify_variables_are_defined() {
    for var in "$@" 
    do
        if [ -z "${!var}" ] 
        then
            echo WARNING: variable ${var} is empty
            echo from function ${FUNCNAME[0]}
            source_trace
            echo terminating 
            exit 1
        fi
        # printf ${fmt} $var${filler:${#var}} ${!var}
        # printf "ZZZZZ  %-${w}s  %s\n" ${var} ${!var}
    done
}
function verify_print_variables() {
    print_varables $@
    verify_variables_are_defined $@

}
