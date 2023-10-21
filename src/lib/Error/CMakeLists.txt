add_library(Error INTERFACE)

target_compile_features(Error INTERFACE cxx_std_20)

target_sources(Error
    PRIVATE
        Exception.hpp
)

target_include_directories(Error INTERFACE "${CMAKE_CURRENT_LIST_DIR}/..")