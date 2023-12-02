add_library(Specification)

target_compile_features(Specification PRIVATE cxx_std_20)

target_sources(Specification
    PRIVATE
        Module.cpp
        Module.hpp
        Common/Utility.cpp
        Common/Utility.hpp
        Common/TableDesc.hpp
        Opal/OpalModule.cpp
        Opal/OpalModule.hpp
        PSID/PSIDModule.cpp
        PSID/PSIDModule.hpp
)

target_sources(Specification
    PRIVATE
        Core/CoreModule.cpp
        Core/CoreModule.hpp
        Core/Defs/TableDescs.hpp
        Core/Defs/UIDs.hpp
)

target_include_directories(Specification INTERFACE "${CMAKE_CURRENT_LIST_DIR}/..")
target_link_libraries(Specification Data)