add_executable(lab6.elf bhTester.c buttonHandler.c flashSequence.c fsTester.c main.c simonControl.c simonDisplay.c verifySequence.c)
target_link_libraries(lab6.elf ${330_LIBS} buttons_switches intervalTimer)
set_target_properties(lab6.elf PROPERTIES LINKER_LANGUAGE CXX)
