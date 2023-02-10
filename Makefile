PRGNAME     = yugi

# define regarding OS, which compiler to use
EXESUFFIX = 
TOOLCHAIN = 
CC          = gcc
CXX         = g++
LD          = gcc

# add SDL dependencies
SDL_LIB     = 
SDL_INCLUDE = 

# change compilation / linking flag options
F_OPTS		= -DHOME_SUPPORT -DNOVA -DUSE_GLES -g -IBase -IGame -IUtility -I/usr/include/qt4/QtGui -I/usr/include/qt4/QtCore -I/usr/include/qt4 -I. -I/usr/include/glm -I.
CC_OPTS		= -O0 -g3 $(F_OPTS)
CFLAGS		= -I$(SDL_INCLUDE) $(CC_OPTS)
CXXFLAGS	= $(CFLAGS)  -std=gnu++11
LDFLAGS     = -lSDL2 -lstdc++ -lGL -lGLU -lGLEW -lpthread -lm

# Files to be compiled
SRCDIR    = ./ThirdParty ./Base ./Game ./Screens ./Screens/Panels ./Utility ./Game/AI ./Game/Animation ./Game/Cards ./Game/Cards/Magic ./Game/Cards/Trap ./Game/Duel ./Game/Duel/Parts
VPATH     = $(SRCDIR)
SRC_C   = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.c))
SRC_CP   = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.cpp))
OBJ_C   = $(notdir $(patsubst %.c, %.o, $(SRC_C)))
OBJ_CP   = $(notdir $(patsubst %.cpp, %.o, $(SRC_CP)))
OBJS     = $(OBJ_C) $(OBJ_CP)

# Rules to make executable
$(PRGNAME)$(EXESUFFIX): $(OBJS)  
	$(LD) $(CFLAGS) -o $(PRGNAME)$(EXESUFFIX) $^ $(LDFLAGS)

$(OBJ_C) : %.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_CP) : %.o : %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(PRGNAME)$(EXESUFFIX) *.o
