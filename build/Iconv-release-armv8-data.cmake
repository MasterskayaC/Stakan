########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

set(libiconv_COMPONENT_NAMES "")
if(DEFINED libiconv_FIND_DEPENDENCY_NAMES)
  list(APPEND libiconv_FIND_DEPENDENCY_NAMES )
  list(REMOVE_DUPLICATES libiconv_FIND_DEPENDENCY_NAMES)
else()
  set(libiconv_FIND_DEPENDENCY_NAMES )
endif()

########### VARIABLES #######################################################################
#############################################################################################
set(libiconv_PACKAGE_FOLDER_RELEASE "/Users/wannabeanything/.conan2/p/b/libic7677c0d27dd7b/p")
set(libiconv_BUILD_MODULES_PATHS_RELEASE )


set(libiconv_INCLUDE_DIRS_RELEASE )
set(libiconv_RES_DIRS_RELEASE )
set(libiconv_DEFINITIONS_RELEASE )
set(libiconv_SHARED_LINK_FLAGS_RELEASE )
set(libiconv_EXE_LINK_FLAGS_RELEASE )
set(libiconv_OBJECTS_RELEASE )
set(libiconv_COMPILE_DEFINITIONS_RELEASE )
set(libiconv_COMPILE_OPTIONS_C_RELEASE )
set(libiconv_COMPILE_OPTIONS_CXX_RELEASE )
set(libiconv_LIB_DIRS_RELEASE "${libiconv_PACKAGE_FOLDER_RELEASE}/lib")
set(libiconv_BIN_DIRS_RELEASE )
set(libiconv_LIBRARY_TYPE_RELEASE STATIC)
set(libiconv_IS_HOST_WINDOWS_RELEASE 0)
set(libiconv_LIBS_RELEASE iconv charset)
set(libiconv_SYSTEM_LIBS_RELEASE )
set(libiconv_FRAMEWORK_DIRS_RELEASE )
set(libiconv_FRAMEWORKS_RELEASE )
set(libiconv_BUILD_DIRS_RELEASE )
set(libiconv_NO_SONAME_MODE_RELEASE FALSE)


# COMPOUND VARIABLES
set(libiconv_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${libiconv_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${libiconv_COMPILE_OPTIONS_C_RELEASE}>")
set(libiconv_LINKER_FLAGS_RELEASE
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${libiconv_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${libiconv_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${libiconv_EXE_LINK_FLAGS_RELEASE}>")


set(libiconv_COMPONENTS_RELEASE )