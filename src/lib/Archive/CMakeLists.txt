add_library(Archive)

target_compile_features(Archive PRIVATE cxx_std_20)

target_sources(Archive
    PRIVATE
        Conversion.hpp
        FlatBinaryArchive.cpp
        FlatBinaryArchive.hpp
        Token.cpp
        Token.hpp
        TokenDebugArchive.cpp
        TokenDebugArchive.hpp
        TokenBinaryArchive.cpp
        TokenBinaryArchive.hpp
)

target_sources(Archive
    PRIVATE
        Types/Exception.hpp
        Types/ValueToJSON.cpp
        Types/ValueToJSON.hpp
        Types/ValueToToken.cpp
        Types/ValueToToken.hpp
        Types/ValueToNative.hpp
)

target_include_directories(Archive INTERFACE "${CMAKE_CURRENT_LIST_DIR}/..")
find_package(cereal REQUIRED)
find_package(nlohmann_json REQUIRED)
target_link_libraries(Archive Data cereal::cereal nlohmann_json::nlohmann_json)