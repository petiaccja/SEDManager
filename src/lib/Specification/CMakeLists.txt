add_library(Specification)

target_compile_features(Specification PRIVATE cxx_std_20)

target_sources(Specification
    PRIVATE
        Authorities.hpp
        Methods.hpp
        Names.cpp
        Names.hpp
        SecurityProviders.hpp
        Tables.hpp
        Types.hpp
)

target_include_directories(Specification INTERFACE "${CMAKE_CURRENT_LIST_DIR}/..")