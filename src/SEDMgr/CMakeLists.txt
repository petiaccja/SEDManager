add_executable(SEDMgr)

target_compile_features(SEDMgr PRIVATE cxx_std_20)

target_sources(SEDMgr
    PRIVATE
        main.cpp
        App.cpp
        App.hpp
)

target_link_libraries(SEDMgr TPerLib)

find_package(CLI11)
target_link_libraries(SEDMgr CLI11::CLI11)