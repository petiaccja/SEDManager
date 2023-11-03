add_library(MockDevice)

target_compile_features(MockDevice PRIVATE cxx_std_20)

target_sources(MockDevice
    PRIVATE
        MockDevice.cpp
        MockDevice.hpp
        MockPreconfig.cpp
        MockPreconfig.hpp
        MockSession.cpp
        MockSession.hpp
        MockSecurityProvider.cpp
        MockSecurityProvider.hpp
)

target_include_directories(MockDevice INTERFACE "${CMAKE_CURRENT_LIST_DIR}/..")
target_link_libraries(MockDevice TrustedPeripheral StorageDevice Specification Archive Data)