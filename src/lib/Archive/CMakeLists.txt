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
        TokenArchive.cpp
        TokenArchive.hpp
)

target_include_directories(Archive INTERFACE "${CMAKE_CURRENT_LIST_DIR}/..")
find_package(cereal REQUIRED)
target_link_libraries(Archive cereal::cereal)