add_library(TPerLib)

target_compile_features(TPerLib PRIVATE cxx_std_20)

target_sources(TPerLib
    PRIVATE
        NvmeDevice.cpp
        NvmeDevice.hpp
        TrustedPeripheral.cpp
        TrustedPeripheral.hpp
        SessionManager.cpp
        SessionManager.hpp

        Rpc/Method.cpp
        Rpc/Method.hpp
        Rpc/MethodArgs.cpp
        Rpc/MethodArgs.hpp

        Serialization/FlatBinaryArchive.hpp
        Serialization/FlatBinaryArchive.cpp
        Serialization/TokenArchive.hpp
        Serialization/TokenArchive.cpp
        Serialization/TokenDebugArchive.hpp
        Serialization/TokenDebugArchive.cpp
        Serialization/Utility.hpp

        Communication/TokenStream.cpp
        Communication/TokenStream.hpp
        Communication/Discovery.hpp
        Communication/Discovery.cpp
        Communication/ComIdSetup.hpp
        Communication/Packet.hpp
        Communication/Token.cpp
        Communication/Token.hpp
)

find_package(cereal REQUIRED)

target_link_libraries(TPerLib cereal::cereal)
target_include_directories(TPerLib INTERFACE "${CMAKE_CURRENT_LIST_DIR}/..")