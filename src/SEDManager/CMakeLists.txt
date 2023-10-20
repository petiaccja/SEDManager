add_executable(SEDManager)

target_compile_features(SEDManager PRIVATE cxx_std_20)

target_sources(SEDManager
    PRIVATE
        main.cpp
        SEDManager.cpp
        SEDManager.hpp
        Object.hpp
        Table.cpp
        Table.hpp
)

target_link_libraries(SEDManager TrustedPeripheral)
find_package(CLI11)
target_link_libraries(SEDManager CLI11::CLI11)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    target_sources(SEDManager PRIVATE admin.manifest)
    target_link_options(SEDManager PRIVATE "/MANIFESTUAC:NO")
endif()