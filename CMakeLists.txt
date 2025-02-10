cmake_minimum_required(VERSION 3.18)
project(ThorvgMultiCanvas)

set(CMAKE_CXX_STANDARD 20)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    link_directories("D:\\sdk\\thorvg\\build\\debug")
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    link_directories("D:\\sdk\\thorvg\\build\\release")
endif()

include_directories("D:\\sdk\\thorvg\\inc")
add_executable(ThorvgMultiCanvas WIN32)

target_sources(ThorvgMultiCanvas PRIVATE 
	main.cpp
	MainWindow.h
	MainWindow.cpp
)


if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
    target_compile_definitions(ThorvgMultiCanvas PRIVATE $<$<CONFIG:Debug>:UNICODE _UNICODE DEBUG NOMINMAX _SILENCE_ALL_CXX20_DEPRECATION_WARNINGS WIN32 _WINDOWS>)
    target_compile_definitions(ThorvgMultiCanvas PRIVATE $<$<CONFIG:Release>:UNICODE _UNICODE NDEBUG NOMINMAX _SILENCE_ALL_CXX20_DEPRECATION_WARNINGS WIN32 _WINDOWS>)
    TARGET_LINK_LIBRARIES(ThorvgMultiCanvas libthorvg glu32 opengl32 dwmapi winmm kernel32 user32 shell32 imm32 gdi32 usp10 shcore)
elseif(APPLE)
endif()