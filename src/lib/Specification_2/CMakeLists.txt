add_library(Specification_2)

target_compile_features(Specification_2 PRIVATE cxx_std_20)

target_sources(Specification_2
    PRIVATE
        Module.cpp
        Module.hpp
        Utility.hpp
        Opal/OpalModule.cpp
        Opal/OpalModule.hpp
        PSID/PSIDModule.cpp
        PSID/PSIDModule.hpp
)

target_sources(Specification_2
    PRIVATE
        Core/CoreModule.cpp
        Core/CoreModule.hpp
        Core/Data/TableDescs.hpp
        Core/Data/Types.hpp
        Core/Data/UIDs.hpp
)

target_include_directories(Specification_2 INTERFACE "${CMAKE_CURRENT_LIST_DIR}/..")
target_link_libraries(Specification_2 Data)