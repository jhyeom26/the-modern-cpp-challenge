add_executable(problem71
    observable_vector.h
    observable_vector_test.cc
)

target_compile_features(problem71 PRIVATE cxx_std_17)
set_target_properties(problem71 PROPERTIES OUTPUT_NAME testapp)
