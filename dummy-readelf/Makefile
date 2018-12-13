CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99 -pedantic -g -I include
SRC = src/read_elf.c src/display_header.c src/display_section_header.c \
	  src/display_program_header.c src/display_symtabs.c \
	  src/display_dynamic_section.c src/utils.c
EXEC = dummy-readelf


all:
	$(CC) $(CFLAGS) $(SRC) -o $(EXEC)

clean:
	$(RM) $(EXEC)
