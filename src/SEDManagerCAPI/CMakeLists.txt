add_library(SEDManagerCAPI SHARED)

target_compile_features(SEDManagerCAPI PRIVATE cxx_std_20)

target_sources(SEDManagerCAPI
    PRIVATE
        SEDManagerCAPI.cpp
        SEDManagerCAPI_2.cpp
)

target_link_libraries(SEDManagerCAPI SEDManager MockDevice)