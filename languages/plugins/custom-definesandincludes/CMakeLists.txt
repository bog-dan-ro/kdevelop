add_subdirectory(tests)
add_subdirectory(compilerprovider)
add_subdirectory(noprojectincludesanddefines)

set( kdevdefinesandincludesmanager_SRCS
        definesandincludesmanager.cpp
        debugarea.cpp
        kcm_widget/projectpathsmodel.cpp
        kcm_widget/definesmodel.cpp
        kcm_widget/includesmodel.cpp
        kcm_widget/includeswidget.cpp
        kcm_widget/defineswidget.cpp
        kcm_widget/projectpathswidget.cpp
        kcm_widget/definesandincludesconfigpage.cpp
        kcm_widget/compilersmodel.cpp
        kcm_widget/compilerswidget.cpp
        kcm_widget/parserwidget.cpp
        compilerprovider/icompiler.cpp # TODO: is this really neccessary
    )

ki18n_wrap_ui(kdevdefinesandincludesmanager_SRCS
    kcm_widget/batchedit.ui
    kcm_widget/includeswidget.ui
    kcm_widget/defineswidget.ui
    kcm_widget/projectpathswidget.ui
    kcm_widget/compilerswidget.ui
    kcm_widget/parserwidget.ui
)
kconfig_add_kcfg_files( kdevdefinesandincludesmanager_SRCS kcm_widget/customdefinesandincludes.kcfgc)

kdevplatform_add_plugin(kdevdefinesandincludesmanager JSON kdevdefinesandincludesmanager.json SOURCES ${kdevdefinesandincludesmanager_SRCS})
target_link_libraries( kdevdefinesandincludesmanager LINK_PRIVATE
        KDev::Project
        KDev::Util
        KDev::Language
        kdevnoprojectincludesanddefines
        kdevcompilerprovider)

option(BUILD_kdev_includepathsconverter "Build utility to modify include paths of a project from command line." ON)
if(BUILD_kdev_includepathsconverter)
    add_executable(kdev_includepathsconverter includepathsconverter.cpp)
    target_link_libraries(kdev_includepathsconverter LINK_PRIVATE
        ${KDEVPLATFORM_PROJECT_LIBRARIES}
        kdevcompilerprovider
    )
    install(TARGETS kdev_includepathsconverter ${INSTALL_TARGETS_DEFAULT_ARGS} )
endif(BUILD_kdev_includepathsconverter)


install(FILES
    idefinesandincludesmanager.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/kdevelop/custom-definesandincludes COMPONENT Devel
)
