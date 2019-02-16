add_executable(test main.cc)

target_compile_features(test PRIVATE cxx_std_17)

find_package(Threads)
target_link_libraries(test PRIVATE Threads::Threads)
