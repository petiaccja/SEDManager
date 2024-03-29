add_library(TrustedPeripheral)

target_compile_features(TrustedPeripheral PRIVATE cxx_std_20)

target_sources(TrustedPeripheral
    PRIVATE
        Discovery.cpp
        Discovery.hpp
        Logging.cpp
        Logging.hpp
        Session.cpp
        Session.hpp
        SessionManager.cpp
        SessionManager.hpp
        TrustedPeripheral.cpp
        TrustedPeripheral.hpp
        MethodUtils.cpp
        MethodUtils.hpp
        ModuleCollection.cpp
        ModuleCollection.hpp
)

target_include_directories(TrustedPeripheral INTERFACE "${CMAKE_CURRENT_LIST_DIR}/..")
target_link_libraries(TrustedPeripheral StorageDevice Specification Archive Messaging)

find_package(asyncpp REQUIRED)
target_link_libraries(TrustedPeripheral asyncpp::asyncpp)