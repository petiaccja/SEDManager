add_executable(Test)

target_sources(Test
    PRIVATE
        Archive/TestTokenArchive.cpp
        Archive/TestValueToJSON.cpp
        Archive/TestValueToNative.cpp
        Data/TestComPacket.cpp
        Data/TestType.cpp
        Data/TestValue.cpp
        main.cpp
        Specification/TestUtility.cpp
        Specification/TestModule.cpp
        TrustedPeripheral/TestMethod.cpp
)

find_package(Catch2 REQUIRED)

target_link_libraries(Test Archive Data Specification TrustedPeripheral)
target_link_libraries(Test Catch2::Catch2)