add_executable(problem66 main.cc)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

find_package(Threads REQUIRED)

target_link_libraries(problem66
    PRIVATE
        CONAN_PKG::spdlog
        Threads::Threads)

target_compile_features(problem66 PRIVATE cxx_std_17)
set_target_properties(problem66 PROPERTIES OUTPUT_NAME testapp)
