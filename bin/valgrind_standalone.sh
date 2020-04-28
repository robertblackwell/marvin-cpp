this_file=$(realpath $0)
bin_dir=$(dirname $this_file)
project_dir=$(realpath ${bin_dir}/../)

echo this_file  : $this_file
echo bin_dir    :$bin_dir
echo project_dir: $project_dir

# MARVIN_HOME=${project_dir} ${project_dir}/cmake-build-debug/apps/handshake/handshake ${marvin_arg} --host ${1}

test="${project_dir}/cmake-build-debug/apps/handshake/handshake --marvin --host www.geeksforgeeks.org"
echo $test
MARVIN_HOME=${project_dir} valgrind --leak-check=full --leak-resolution=low --undef-value-errors=no  $test