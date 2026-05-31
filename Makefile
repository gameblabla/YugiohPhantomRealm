PRGNAME     = yugi.elf

CC          = gcc
CXX         = g++
LD          = gcc
OPENGLES	:= FALSE

# change compilation / linking flag options
F_OPTS		= -DHOME_SUPPORT -g -IBase -IGame -IUtility -I. -I/usr/include/glm -I.

#Only define for OpenGL ES 2.x mode
CFLAGS		= -O0 -g3 $(F_OPTS)

ifeq ($(OPENGLES), TRUE)
CFLAGS 		+= -DNOVA -DUSE_GLES
endif

CXXFLAGS	= $(CFLAGS)  -std=gnu++11
LDFLAGS     = -lSDL2 -lSDL2_mixer -lstdc++ -lpthread -lm

ifeq ($(OPENGLES), TRUE)
LDFLAGS 	+= -lGLESv2 -lGLU
endif

ifeq ($(OPENGLES), FALSE)
LDFLAGS 	+= -lGL -lGLU -lGLEW
endif

# Files to be compiled
SRCDIR    = ./ThirdParty ./Base ./Game ./Screens ./Screens/Panels ./Utility ./Game/AI ./Game/Animation ./Game/Cards ./Game/Cards/Magic ./Game/Cards/Trap ./Game/Duel ./Game/Duel/Parts
VPATH     = $(SRCDIR)
SRC_C   = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.c))
SRC_CP   = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.cpp))
OBJ_C   = $(notdir $(patsubst %.c, %.o, $(SRC_C)))
OBJ_CP   = $(notdir $(patsubst %.cpp, %.o, $(SRC_CP)))
OBJS     = $(OBJ_C) $(OBJ_CP)

# Rules to make executable
$(PRGNAME): $(OBJS)  
	$(CXX) $(CXXFLAGS) -o $(PRGNAME) $^ $(LDFLAGS)

$(OBJ_C) : %.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_CP) : %.o : %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(PRGNAME) *.o
