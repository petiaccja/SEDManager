add_executable(SEDManager)

target_compile_features(SEDManager PRIVATE cxx_std_20)

target_sources(SEDManager
    PRIVATE
        main.cpp
        App.cpp
        App.hpp
)

target_link_libraries(SEDManager TrustedPeripheral)
find_package(CLI11)
target_link_libraries(SEDManager CLI11::CLI11)