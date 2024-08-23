include("${CMAKE_CURRENT_LIST_DIR}/CommonLibSSE-targets.cmake")
include(CMakeFindDependencyMacro)

find_dependency(binary_io CONFIG)
find_dependency(spdlog CONFIG)
