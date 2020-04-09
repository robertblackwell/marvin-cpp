project_dir=$(realpath ./)
build_dir=${project_dir}/cmake-build-debug
config_prog=${build_dir}/apps/config/config

echo ${project_dir}
echo ${config_prog}

MARVIN_HOME=${project_dir} ${config_prog}