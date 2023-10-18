add_executable(Test)

target_sources(Test
    PRIVATE
        main.cpp
        Archive/TestTokenArchive.cpp
        Archive/TestPacket.cpp
        RPC/TestValue.cpp
        RPC/TestMethod.cpp
        RPC/TestValueCast.cpp
        Specification/TestType.cpp
        UI/TestValueToJSON.cpp
)

find_package(Catch2 REQUIRED)

target_link_libraries(Test Archive Data Specification TrustedPeripheral)
target_link_libraries(Test Catch2::Catch2)