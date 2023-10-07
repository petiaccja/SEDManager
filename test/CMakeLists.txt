add_executable(Test)

target_sources(Test
    PRIVATE
        main.cpp
        TestTokenArchive.cpp
        TestPackets.cpp
        TestValue.cpp
        TestMethod.cpp
)

find_package(Catch2 REQUIRED)

target_link_libraries(Test TPerLib)
target_link_libraries(Test Catch2::Catch2)