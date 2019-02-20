add_executable(problem61 main.cc)

find_package(Threads REQUIRED)
target_link_libraries(problem61 PRIVATE Threads::Threads)

target_compile_features(problem61 PRIVATE cxx_std_17)
set_target_properties(problem61 PROPERTIES OUTPUT_NAME testapp)
