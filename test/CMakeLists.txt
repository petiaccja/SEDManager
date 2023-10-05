add_executable(Test)

target_sources(Test
    PRIVATE
        main.cpp
        TestTokenStreamArchive.cpp
        TestPackets.cpp
        TestRpcStream.cpp
)

find_package(Catch2 REQUIRED)

target_link_libraries(Test TPerLib)
target_link_libraries(Test Catch2::Catch2)