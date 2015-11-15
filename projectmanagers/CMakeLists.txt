ecm_optional_add_subdirectory(cmake)
ecm_optional_add_subdirectory(custommake)
ecm_optional_add_subdirectory(custom-buildsystem)
add_subdirectory(qbs)
if (KDevelop-PG-Qt_FOUND)
    ecm_optional_add_subdirectory(qmake)
endif()

