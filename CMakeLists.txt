project( ChromiumUpdaterQt )
cmake_minimum_required( VERSION 3.2 )
set ( CMAKE_BUILD_TYPE Release )
add_definitions ( -Wall )

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
find_package ( Qt5Core )
find_package ( Qt5Widgets )
find_package ( Qt5Network )

set ( ChromiumUpdaterQt_SRCS main.cpp chromiumupdaterwidget.h chromiumupdater.hpp chromiumupdaterwidget.cpp )

add_executable ( ${PROJECT_NAME} WIN32 ${ChromiumUpdaterQt_SRCS} )
target_link_libraries ( ${PROJECT_NAME} Qt5::WinMain Qt5::Widgets Qt5::Network )

if(MSVC) # Check if we are using the Visual Studio compiler
	set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
endif()
