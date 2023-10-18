add_library(Data)

target_compile_features(Data PRIVATE cxx_std_20)

target_sources(Data
    PRIVATE
        ComPacket.hpp
        Value.cpp
        Value.hpp
        Type.hpp
        NativeTypes.hpp
        SetupPackets.hpp
)

target_include_directories(Data INTERFACE "${CMAKE_CURRENT_LIST_DIR}/..")