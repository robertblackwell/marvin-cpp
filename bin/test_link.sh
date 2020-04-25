
# /usr/bin/c++ \
#     -frtti -fvisibility-inlines-hidden -fvisibility=hidden -pthread -g   \
#     CMakeFiles/test_certificates.dir/test_certificates.cpp.o  \
#     -o test_certificates  \
#     ../../../marvin/libmarvin_library.a \
#     ../../../marvin/libmarvin_library.a \
#     -ldl \
#     ../../../../vendor/lib/libcrypto.a \
#     ../../../../vendor/lib/libssl.a \
#     ../../../../vendor/lib/libboost_filesystem.a \
#     ../../../../vendor/lib/libboost_system.a \
#     ../../../../vendor/lib/libcert_library.a \
#     -ldl -lpthread \
#     ../../../../vendor/lib/libcrypto.a \
#     ../../../../vendor/lib/libssl.a \
#     ../../../../vendor/lib/libboost_filesystem.a \
#     ../../../../vendor/lib/libboost_system.a \
#     ../../../../vendor/lib/libcert_library.a 

project_dir=/home/robert/Projects/marvin++
/usr/bin/c++ \
    -frtti -fvisibility-inlines-hidden -fvisibility=hidden -pthread -g   \
    cmake-build-debug/tests/test_certificates/test_certificates/CMakeFiles/test_certificates.dir/test_certificates.cpp.o  \
    -o test_certificates  \
    ${project_dir}/cmake-build-debug/marvin/libmarvin_library.a \
    -lpthread \
    ${project_dir}/vendor/lib/libboost_filesystem.a \
    ${project_dir}/vendor/lib/libboost_system.a \
    ${project_dir}/vendor/lib/libcert_library.a \
    ${project_dir}/vendor/lib/libssl.a \
    ${project_dir}/vendor/lib/libcrypto.a \
    -ldl

