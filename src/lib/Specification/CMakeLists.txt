add_library(Specification)

target_compile_features(Specification PRIVATE cxx_std_20)

target_sources(Specification
    PRIVATE
        ColumnTypes.cpp
        ColumnTypes.hpp
        Enumerations.hpp
        Identifiers.hpp
        Names.cpp
        Names.hpp
        NativeTypes.hpp
        Tables.cpp
        Tables.hpp
        Type.hpp
)

target_include_directories(Specification INTERFACE "${CMAKE_CURRENT_LIST_DIR}/..")