add_subdirectory(kcm_widget)
add_subdirectory(tests)

include_directories( ${KDE4_INCLUDES} ${KDEVPLATFORM_INCLUDE_DIR} )

add_definitions( ${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

set( kdevdefinesandincludesmanager_SRCS
        definesandincludesmanager.cpp
        settingsmanager.cpp
        debugarea.cpp
    )
kde4_add_plugin( kdevdefinesandincludesmanager
        ${kdevdefinesandincludesmanager_SRCS})
target_link_libraries( kdevdefinesandincludesmanager LINK_PRIVATE
        ${KDEVPLATFORM_PROJECT_LIBRARIES}
        ${KDEVPLATFORM_LANGUAGE_LIBRARIES} )

install( TARGETS kdevdefinesandincludesmanager
         DESTINATION ${PLUGIN_INSTALL_DIR} )

configure_file(kdevdefinesandincludesmanager.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kdevdefinesandincludesmanager.desktop)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kdevdefinesandincludesmanager.desktop DESTINATION ${SERVICES_INSTALL_DIR} )