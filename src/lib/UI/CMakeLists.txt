add_library(UI)

target_compile_features(UI PRIVATE cxx_std_20)

target_sources(UI
    PRIVATE
        ValueToJSON.hpp
        ValueToJSON.cpp
)

target_include_directories(UI INTERFACE "${CMAKE_CURRENT_LIST_DIR}/..")
target_link_libraries(UI RPC Specification)
find_package(nlohmann_json REQUIRED)
target_link_libraries(UI nlohmann_json::nlohmann_json)