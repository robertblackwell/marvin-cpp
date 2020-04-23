# runs test_doctest with cwd of Project directory
echo Just to show we got here
echo Just to show we got here
echo Just to show we got here
echo Just to show we got here
echo Just to show we got here
echo Just to show we got here
echo Just to show we got here
doctest_dir=$(realpath ./)
project_dir=/home/robert/Projects/marvin++
cd ${project_dir}
${doctest_dir}/test_doctest