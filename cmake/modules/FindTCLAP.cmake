SET (TCLAP_FOUND FALSE)

IF(EXISTS TCLAP_INCLUDE_DIR)  # already found

    IF( TCLAP_INCLUDE_DIR )
        SET(TCLAP_FOUND TRUE)
    ENDIF( TCLAP_INCLUDE_DIR )

ELSE(EXISTS TCLAP_INCLUDE_DIR)
    FIND_PATH(TCLAP_INCLUDE_DIR tclap/CmdLine.h
            /usr/include /opt/local/include
            )

    IF( TCLAP_INCLUDE_DIR )
        SET(TCLAP_FOUND TRUE)
    ENDIF( TCLAP_INCLUDE_DIR )

ENDIF(EXISTS TCLAP_INCLUDE_DIR)