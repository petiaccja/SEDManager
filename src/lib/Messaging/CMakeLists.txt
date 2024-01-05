add_library(Messaging)

target_compile_features(Messaging PRIVATE cxx_std_20)

target_sources(Messaging
    PRIVATE
        ComPacket.hpp
        Value.cpp
        Value.hpp
        Type.hpp
        NativeTypes.cpp
        NativeTypes.hpp
        SetupPackets.hpp
        TokenStream.cpp
        TokenStream.hpp
        Token.cpp
        Token.hpp
        Method.cpp
        Method.hpp
)

target_include_directories(Messaging INTERFACE "${CMAKE_CURRENT_LIST_DIR}/..")
target_link_libraries(Messaging Archive Error)