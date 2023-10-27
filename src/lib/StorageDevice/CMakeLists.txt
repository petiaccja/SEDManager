add_library(StorageDevice)

target_compile_features(StorageDevice PRIVATE cxx_std_20)


target_sources(StorageDevice
    PRIVATE
        NvmeDevice.hpp
        StorageDevice.hpp
        Common/NvmeStructures.hpp
)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    target_sources(StorageDevice
        PRIVATE
            Windows/NvmeDevice.cpp
            Windows/NvmeDevice.hpp
            Windows/EnumerateStorageDevices.cpp
    )
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_sources(StorageDevice
        PRIVATE
            Linux/NvmeDevice.cpp
            Linux/NvmeDevice.hpp
            Linux/EnumerateStorageDevices.cpp
    )
else()
    message(FATAL_ERROR "Storage devices have no implementation for CMake system ${CMAKE_SYSTEM_NAME}")
endif()

target_include_directories(StorageDevice INTERFACE "${CMAKE_CURRENT_LIST_DIR}/..")
target_link_libraries(StorageDevice Error)