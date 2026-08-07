# Best-effort local-dev deploy step, run as a POST_BUILD step on the plugin
# target. Copies (and, the first time, imports/registers) the freshly built
# plugin into a sibling openIndy app build, so a local `openIndy` checkout
# picks up plugin changes without a manual "Import Plugin" pass through the
# GUI every time.
#
# Expects PLUGIN_SO, APP_DIR and OPENINDY_EXE to be passed via -D.

if(NOT EXISTS "${OPENINDY_EXE}")
    message(STATUS "DeployToApp: openIndy executable not found at ${OPENINDY_EXE} - skipping plugin auto-deploy (build the openindy app first, see OI_APP_LOCAL_DIR)")
    return()
endif()

get_filename_component(PLUGIN_FILENAME "${PLUGIN_SO}" NAME)
set(DEST_PLUGIN "${APP_DIR}/plugins/${PLUGIN_FILENAME}")

if(EXISTS "${DEST_PLUGIN}")
    # Already imported/registered in oisystemdb.sqlite in a previous build -
    # PluginCopier::importPlugin refuses to re-import over an existing file,
    # so just refresh the binary in place; the app picks it up on next launch.
    configure_file("${PLUGIN_SO}" "${DEST_PLUGIN}" COPYONLY)
    message(STATUS "DeployToApp: refreshed ${DEST_PLUGIN}")
else()
    # First time - run the app's real import pipeline so it both copies the
    # file and registers it in oisystemdb.sqlite (a plain file copy alone
    # would not make the app load it).
    #
    # WORKING_DIRECTORY matters here, not just cosmetically: openIndy's own
    # app-directory/DB-path resolution silently comes up empty when invoked
    # from outside its own bin dir (exit code 0, plugin file copied, but no
    # row ever written to oisystemdb.sqlite) - confirmed empirically, root
    # cause not chased further since forcing this sidesteps it entirely.
    execute_process(
        COMMAND "${OPENINDY_EXE}" -i "${PLUGIN_SO}"
        WORKING_DIRECTORY "${APP_DIR}"
        RESULT_VARIABLE IMPORT_RESULT
        OUTPUT_QUIET
        ERROR_QUIET
    )
    if(NOT IMPORT_RESULT EQUAL 0)
        message(WARNING "DeployToApp: auto-import into openIndy app failed (exit ${IMPORT_RESULT}) - import it manually via Settings > Plugin Manager")
        return()
    endif()

    # exit 0 from the importer isn't proof the DB row actually landed (see
    # note above) - verify it directly when sqlite3 is available so a silent
    # failure here doesn't masquerade as success.
    find_program(OI_SQLITE3_EXE sqlite3)
    if(OI_SQLITE3_EXE)
        execute_process(
            COMMAND "${OI_SQLITE3_EXE}" "${APP_DIR}/oisystemdb.sqlite"
                "SELECT count(*) FROM plugin WHERE file_path='plugins/${PLUGIN_FILENAME}';"
            OUTPUT_VARIABLE OI_PLUGIN_ROW_COUNT
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        if(NOT OI_PLUGIN_ROW_COUNT GREATER 0)
            message(WARNING "DeployToApp: openIndy -i reported success but no row was written to oisystemdb.sqlite - import it manually via Settings > Plugin Manager")
            return()
        endif()
    endif()

    message(STATUS "DeployToApp: imported and registered plugin at ${DEST_PLUGIN}")
endif()
