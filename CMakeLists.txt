cmake_minimum_required (VERSION 3.19)
project("${CMAKE_PROJECT_NAME}")
include("$ENV{TOOLCHAIN_PATH}/CMakeLists.txt.in")

add_lib(libasp "source/lib")
add_bin(aspview "source/bin/view" libasp ${LIB_CFG} ${LIB_SHM} ${LIB_LOG} ${LIB_COM})
add_bin(loadasp "source/bin/load" libasp ${LIB_CFG} ${LIB_SHM} ${LIB_LOG} ${LIB_COM})
add_unittest(libasp ${LIB_SHM} ${LIB_LOG} ${LIB_COM})
