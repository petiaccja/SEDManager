add_library(TrustedPeripheral)

target_compile_features(TrustedPeripheral PRIVATE cxx_std_20)

target_sources(TrustedPeripheral
    PRIVATE
        Discovery.cpp
        Discovery.hpp
        Exception.hpp
        Logging.cpp
        Logging.hpp
        Session.cpp
        Session.hpp
        SessionManager.cpp
        SessionManager.hpp
        Setup.hpp
        TrustedPeripheral.cpp
        TrustedPeripheral.hpp
)

target_include_directories(TrustedPeripheral INTERFACE "${CMAKE_CURRENT_LIST_DIR}/..")
target_link_libraries(TrustedPeripheral StorageDevice)
target_link_libraries(TrustedPeripheral Specification)
target_link_libraries(TrustedPeripheral RPC)
target_link_libraries(TrustedPeripheral Archive)
find_package(cereal REQUIRED)
target_link_libraries(TrustedPeripheral cereal::cereal)