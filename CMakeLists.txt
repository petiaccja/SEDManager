cmake_minimum_required(VERSION 3.25)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(SEDManager)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
	if (ENABLE_LLVM_COV)
		add_compile_options("-fprofile-instr-generate" "-fcoverage-mapping" "-mllvm" "-enable-name-compression=false")
		add_link_options("-fprofile-instr-generate" "-fcoverage-mapping")
	endif()
	if (ENABLE_LLVM_ADDRESS_SANITIZER)
		add_compile_options("-fsanitize=address")
		add_link_options("-fsanitize=address")
	endif()
	if (ENABLE_LLVM_MEMORY_SANITIZER)
		add_compile_options("-fsanitize=memory")
		add_link_options("-fsanitize=memory")
	endif()
	add_compile_options("-Wno-shift-count-overflow")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
	add_compile_options("/utf-8")
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(src)
add_subdirectory(test)

install(TARGETS SEDManagerCLI)