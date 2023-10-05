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
        Serialization/FlatBinaryArchive.hpp
        Serialization/FlatBinaryArchive.cpp
        Serialization/TokenStreamArchive.hpp
        Serialization/TokenStreamArchive.cpp
        Serialization/Utility.hpp
        Serialization/RpcStream.cpp
        Serialization/RpcStream.hpp
        Structures/CommLayer.hpp
        Structures/Discovery.hpp
        Structures/Discovery.cpp
)

find_package(cereal REQUIRED)

target_link_libraries(TPerLib cereal::cereal)
target_include_directories(TPerLib INTERFACE "${CMAKE_CURRENT_LIST_DIR}/..")