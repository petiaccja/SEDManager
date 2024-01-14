add_library(SEDManagerCAPI SHARED)

target_compile_features(SEDManagerCAPI PRIVATE cxx_std_20)

target_sources(SEDManagerCAPI
    PRIVATE        
        SEDManagerCAPI.cpp
)

target_link_libraries(SEDManagerCAPI EncryptedDevice MockDevice)