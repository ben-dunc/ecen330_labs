add_executable(lab5.elf main.c minimax.c testBoards.c ticTacToeControl.c ticTacToeDisplay.c)
target_link_libraries(lab5.elf ${330_LIBS} buttons_switches intervalTimer)
set_target_properties(lab5.elf PROPERTIES LINKER_LANGUAGE CXX)
