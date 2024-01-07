add_library(Archive)

target_compile_features(Archive PRIVATE cxx_std_20)

target_sources(Archive
    PRIVATE
        Serialization.hpp
        FlatBinaryArchive.cpp
        FlatBinaryArchive.hpp
)

target_sources(Archive
    PRIVATE
        Types/ValueToJSON.cpp
        Types/ValueToJSON.hpp
)

target_include_directories(Archive INTERFACE "${CMAKE_CURRENT_LIST_DIR}/..")
find_package(cereal REQUIRED)
find_package(nlohmann_json REQUIRED)
target_link_libraries(Archive Error cereal::cereal nlohmann_json::nlohmann_json)