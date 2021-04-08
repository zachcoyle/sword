#####################################################################
# First, install the library itself
# 
MESSAGE(STATUS "\n-- CONFIGURING INSTALLATION DESTINATIONS")
# Configuration files, of course

# Install the library
IF(BUILDING_SHARED)
	INSTALL(TARGETS sword
        RUNTIME DESTINATION "${BINDIR}"
        LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
        ARCHIVE DESTINATION "${LIB_INSTALL_DIR}")
ENDIF(BUILDING_SHARED)
IF(BUILDING_STATIC)
	INSTALL(TARGETS sword_static
        RUNTIME DESTINATION "${BINDIR}"
        LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
        ARCHIVE DESTINATION "${LIB_INSTALL_DIR}")
ENDIF(BUILDING_STATIC)
# Install the locales
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/locales.d
    DESTINATION "${SHARE_INSTALL_PREFIX}/sword")

# Install the headers
INSTALL(FILES ${SWORD_INSTALL_HEADERS}
    DESTINATION "${INCLUDE_INSTALL_DIR}/sword")
# Install sysconf file
INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/sword.conf"
    DESTINATION "${SYSCONF_INSTALL_DIR}")
# Create the mods.d
INSTALL(DIRECTORY DESTINATION "${SHARE_INSTALL_PREFIX}/sword/mods.d")

IF(SWORD_INTERNAL_REGEX)
	INSTALL(FILES ${INTERNAL_REGEX_HEADER}
        DESTINATION "${INCLUDE_INSTALL_DIR}/sword")
ENDIF(SWORD_INTERNAL_REGEX)

MESSAGE(STATUS "Destination: ${CMAKE_INSTALL_PREFIX}")

SET(VERSION		${SWORD_VERSION})
IF(WITH_CURL)
	SET(CURL_LIBS	${CURL_LIBRARY})
ENDIF(WITH_CURL)
IF(WITH_CLUCENE)
	SET(CLUCENE_LIBS	${CLUCENE_LIBRARY})
ENDIF(WITH_CLUCENE)
IF(WITH_ICU)
	SET(ICU_LIBS	${ICU_LIBRARY})
ENDIF(WITH_ICU)

IF(LIBSWORD_LIBRARY_TYPE STREQUAL "Static")
	SET(SHAREDLIB_TRUE "#")
ELSE(LIBSWORD_LIBRARY_TYPE STREQUAL "Static")
	SET(SHAREDLIB_FALSE "#")
ENDIF(LIBSWORD_LIBRARY_TYPE STREQUAL "Static")
# The @ONLY restricts it because our ${variable} which are left there as part of pkg-config
SET(prefix ${CMAKE_INSTALL_PREFIX})
SET(libdir ${LIB_INSTALL_DIR})
SET(exec_prefix ${BINDIR})
SET(includedir ${INCLUDE_INSTALL_DIR})
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/sword.pc.in ${CMAKE_CURRENT_BINARY_DIR}/sword.pc @ONLY)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/include/swversion.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/swversion.h @ONLY)

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/sword.pc
    DESTINATION "${LIB_INSTALL_DIR}/pkgconfig")
