add_library(SEDManager)

target_compile_features(SEDManager PRIVATE cxx_std_20)

target_sources(SEDManager
    PRIVATE
        SEDManager.cpp
        SEDManager.hpp
)

target_include_directories(SEDManager INTERFACE "${CMAKE_CURRENT_LIST_DIR}/..")
target_link_libraries(SEDManager TrustedPeripheral)