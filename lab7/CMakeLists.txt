add_executable(lab7.elf globals.c main.c gameControl.c pongControl.c)
target_link_libraries(lab7.elf ${330_LIBS} buttons_switches)
set_target_properties(lab7.elf PROPERTIES LINKER_LANGUAGE CXX)
