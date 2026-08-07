# Deploy step, run as a POST_BUILD step on the plugin target. Copies the
# freshly built plugin into a sibling openIndy app build's plugins/
# directory, so a local `openIndy` checkout picks up plugin changes without
# a manual step every time. PluginRepository scans that directory itself at
# startup - there's no separate registration step to run.
#
# Expects PLUGIN_SO and APP_DIR to be passed via -D.

if(NOT EXISTS "${APP_DIR}")
    message(STATUS "DeployToApp: app directory not found at ${APP_DIR} - skipping plugin auto-deploy (build the openindy app first, see OI_APP_LOCAL_DIR)")
    return()
endif()

get_filename_component(PLUGIN_FILENAME "${PLUGIN_SO}" NAME)
set(DEST_PLUGIN "${APP_DIR}/plugins/${PLUGIN_FILENAME}")

configure_file("${PLUGIN_SO}" "${DEST_PLUGIN}" COPYONLY)
message(STATUS "DeployToApp: deployed ${DEST_PLUGIN}")
