add_executable(test main.cpp)
include(CompilerWarnings)
set_project_warnings(test)
target_link_libraries(test PRIVATE cpop)

