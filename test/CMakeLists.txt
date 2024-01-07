add_executable(Test)

target_sources(Test
    PRIVATE
        main.cpp
        Messaging/TestTokenStream.cpp
        Archive/TestValueToJSON.cpp
        Archive/TestValueToNative.cpp
        Messaging/TestComPacket.cpp
        Messaging/TestUID.cpp
        Messaging/TestType.cpp
        Messaging/TestSetupPackets.cpp
        Messaging/TestValue.cpp
        Specification/TestUtility.cpp
        Specification/TestModule.cpp        
        TrustedPeripheral/TestDiscovery.cpp
        Mock/TestTrustedPeripheral.cpp
        Mock/TestSession.cpp
        Mock/TestSessionManager.cpp
        Messaging/TestMethod.cpp
)

find_package(Catch2 3 REQUIRED)

target_link_libraries(Test Archive Messaging Specification TrustedPeripheral MockDevice EncryptedDevice)
target_link_libraries(Test Catch2::Catch2)