add_library(EncryptedDevice)

target_compile_features(EncryptedDevice PRIVATE cxx_std_20)

target_sources(EncryptedDevice
    PRIVATE
        EncryptedDevice.cpp
        EncryptedDevice.hpp
)

target_include_directories(EncryptedDevice INTERFACE "${CMAKE_CURRENT_LIST_DIR}/..")
target_link_libraries(EncryptedDevice TrustedPeripheral)