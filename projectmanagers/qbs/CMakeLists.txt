project(qbsmanager)
add_definitions(-DTRANSLATION_DOMAIN=\"kdevqbs\" -DQBS_ENABLE_PROJECT_FILE_UPDATES)
include_directories(
    ${makebuilder_SOURCE_DIR}
    /home/bogdan/.local/include/qbs
    )


########### next target ###############

set(kdevqbsmanager_PART_SRCS
    qbsmanager.cpp
    qbsitems.cpp
    qbsimportjob.cpp
    qbsprojectconfig.cpp
    qbskjob.cpp
)
ki18n_wrap_ui(kdevqbsmanager_PART_SRCS
    qbsprojectconfig.ui
    )

kdevplatform_add_plugin(kdevqbsmanager JSON kdevqbsmanager.json SOURCES ${kdevqbsmanager_PART_SRCS})
target_link_libraries(kdevqbsmanager
    KF5::TextEditor
    KF5::KIOWidgets
    KDev::Project
    KDev::Util
    KDev::Language
    KDev::OutputView
    KDev::Shell
    kdevmakefileresolver
    KDev::Interfaces
    /home/bogdan/.local/lib/libqbscore.so
    /home/bogdan/.local/lib/libqbsqtprofilesetup.so
)

########### install files ###############

install( FILES kdevqbsmanager.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/kdevqbsmanager )
