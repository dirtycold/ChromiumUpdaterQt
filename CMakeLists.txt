project( ChromiumUpdaterQt )
cmake_minimum_required( VERSION 2.6 )
set ( CMAKE_BUILD_TYPE Release )
add_definitions ( -Wall )
find_package ( Qt4 REQUIRED )
include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTNETWORK_INCLUDE_DIR}
	)
set ( ChromiumUpdaterQt_HDRS )

set ( ChromiumUpdaterQt_SRCS main.cpp chromiumupdaterwidget.cpp )

set ( ChromiumUpdaterQt_MOCS chromiumupdaterwidget.h chromiumupdater.hpp )
QT4_WRAP_CPP(MOCS ${ChromiumUpdaterQt_MOCS})

add_executable ( ChromiumUpdaterQt ${ChromiumUpdaterQt_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries ( ChromiumUpdaterQt ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTNETWORK_LIBRARY} )
