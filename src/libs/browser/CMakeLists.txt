add_library(Browser STATIC
    webcontrol.cpp
    searchtoolbar.cpp
    webbridge.cpp
    webview.cpp
)

target_link_libraries(Browser)

find_package(Qt5 COMPONENTS WebEngineWidgets REQUIRED)
target_link_libraries(Browser Qt5::WebEngineWidgets)
