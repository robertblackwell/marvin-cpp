# runs test_doctest with cwd of Project directory
certificatestest_dir=$(realpath ./)
project_dir=${1} #/home/robert/Projects/marvin++
cd ${project_dir}
echo "From inside test.sh parameter 1 = $1"
${certificatestest_dir}/test_certificates