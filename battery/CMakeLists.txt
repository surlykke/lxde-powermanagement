project(razor-autosuspend)

include_directories ( ${CMAKE_CURRENT_SOURCE_DIR}
                      ${CMAKE_CURRENT_BINARY_DIR}
                      ${CMAKE_SOURCE_DIR}/libraries/
                    )

set ( razor-autosuspend_HDRS
        )

set ( razor-autosuspend_SRCS
        main.cpp
        trayicon.cpp
        razorautosuspend.cpp
        battery.cpp
        batteryinfo.cpp
        )

set ( razor-autosuspend_UIS
      batteryinfo.ui
    )

set ( razor-autosuspend_RSCS
        )
QT4_ADD_RESOURCES(RSCS ${razor-autosuspend_RSCS})

set ( razor-autosuspend_MOCS
      razorautosuspend.h
      trayicon.h
      battery.h
      batteryinfo.h
    )

# TODO: remove actual file and its translations
set(DESKTOP_FILES_IN
#    razor-autosuspend.desktop.in
)

set(CONFIG_FILES
    razor-autosuspend.conf
    )

QT4_WRAP_CPP(MOCS ${razor-autosuspend_MOCS})

QT4_WRAP_UI( UIS ${razor-autosuspend_UIS}
           )


set ( ICON_FILES icons/razor-autosuspend.svg
    )

# Translations **********************************
include(RazorTranslate)
razor_translate_ts(QM_FILES
    SOURCES
        ${razor-autosuspend_HDRS}
        ${razor-autosuspend_SRCS}
        ${razor-autosuspend_UIS}
)

razor_translate_desktop(DESKTOP_FILES
    SOURCES
        ${DESKTOP_FILES_IN}
)
#************************************************

add_executable ( razor-autosuspend ${razor-autosuspend_SRCS} ${UIS} ${RSCS} ${QM_FILES} ${MOCS} ${DESKTOP_FILES})
target_link_libraries ( razor-autosuspend  
                        ${RAZORQT_LIBRARIES}
                        ${RAZORXDG_LIBRARIES}
                        ${QT_QTCORE_LIBRARY} 
                        ${QT_QTGUI_LIBRARY} 
                        ${QT_QTDBUS_LIBRARY} 
                        ${QT_QTSVG_LIBRARY})


install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(FILES   ${DESKTOP_FILES}   DESTINATION share/applications)
install(FILES   ${ICON_FILES}      DESTINATION share/icons/hicolor/scalable/apps)
install(FILES   ${CONFIG_FILES}    DESTINATION ${RAZOR_ETC_XDG_DIRECTORY}/razor)