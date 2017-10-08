# Add sources        "${CMAKE_CURRENT_LIST_DIR}/.cpp"

target_sources(ChessPlusPlus PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/Application.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Button.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/ButtonManager.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/ChessPlusPlusState.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/GameOverState.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/GraphicsHandler.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/StartMenuState.cpp"
        )
