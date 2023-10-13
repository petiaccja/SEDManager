add_library(RPC)

target_compile_features(RPC PRIVATE cxx_std_20)

target_sources(RPC
    PRIVATE
        Exception.hpp
        Method.cpp
        Method.hpp
        Packet.hpp
        Value.cpp
        Value.hpp
        ValueCast.hpp
)

target_include_directories(RPC INTERFACE "${CMAKE_CURRENT_LIST_DIR}/..")
target_link_libraries(RPC Archive)
find_package(cereal REQUIRED)
target_link_libraries(Archive cereal::cereal)