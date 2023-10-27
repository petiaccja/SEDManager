add_executable(SEDManagerCLI)

target_compile_features(SEDManagerCLI PRIVATE cxx_std_20)

target_sources(SEDManagerCLI
    PRIVATE
        main.cpp
        Interactive.cpp
        Interactive.hpp
        PBA.cpp
        PBA.hpp
        Utility.cpp
        Utility.hpp
)

target_link_libraries(SEDManagerCLI SEDManager)
find_package(CLI11)
target_link_libraries(SEDManagerCLI CLI11::CLI11)
find_package(rang)
target_link_libraries(SEDManagerCLI rang::rang)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    target_sources(SEDManagerCLI PRIVATE admin.manifest)
    target_link_options(SEDManagerCLI PRIVATE "/MANIFESTUAC:NO")
endif()