CONTIKI = ../contiki-2.7

# Target file for the project
TARGET = sky
PROJECT_SOURCEFILES +=

# Define the program's main source file (error handling code)
all: error_handling

# Include the Contiki build system
include $(CONTIKI)/Makefile.include
