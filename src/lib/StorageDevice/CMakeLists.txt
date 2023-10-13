add_library(StorageDevice)

target_compile_features(StorageDevice PRIVATE cxx_std_20)

target_sources(StorageDevice
    PRIVATE
        NvmeDevice.cpp
        NvmeDevice.hpp
)

target_include_directories(StorageDevice INTERFACE "${CMAKE_CURRENT_LIST_DIR}/..")