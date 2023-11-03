add_executable(Test)

target_sources(Test
    PRIVATE
        main.cpp
        Archive/TestTokenArchive.cpp
        Archive/TestValueToJSON.cpp
        Archive/TestValueToNative.cpp
        Archive/TestValueToToken.cpp
        Data/TestComPacket.cpp
        Data/TestNativeTypes.cpp
        Data/TestType.cpp
        Data/TestSetupPackets.cpp
        Data/TestValue.cpp
        Specification/TestUtility.cpp
        Specification/TestModule.cpp
        TrustedPeripheral/TestMethod.cpp
        TrustedPeripheral/TestDiscovery.cpp
        Mock/TestTrustedPeripheral.cpp
        Mock/TestSession.cpp
        Mock/TestSessionManager.cpp
)

find_package(Catch2 3 REQUIRED)

target_link_libraries(Test Archive Data Specification TrustedPeripheral MockDevice)
target_link_libraries(Test Catch2::Catch2)