# runs test_doctest with cwd of Project directory
doctest_dir=$(realpath ./)
project_dir=/home/robert/Projects/marvin++
cd ${project_dir}
${doctest_dir}/test_doctest