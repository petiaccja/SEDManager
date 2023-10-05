add_executable(SEDMgr)

target_compile_features(SEDMgr PRIVATE cxx_std_20)

target_sources(SEDMgr
    PRIVATE
        main.cpp
)

target_link_libraries(SEDMgr TPerLib)