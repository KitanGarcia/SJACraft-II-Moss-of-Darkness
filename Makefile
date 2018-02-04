AR=ar
AS=as
CC=gcc
CPP=cpp
CXX=g++
LD=ld
OBJCOPY = objcopy
OBJDUMP = objdump
STRIP = strip

INC_DIR = ./include
SRC_DIR = ./src
OBJ_DIR = ./obj
BIN_DIR = ./bin

# Don't print the commands unless explicitely requested with `make V=1`
ifneq ($(V),1)
Q = @
V = 0
endif

ifneq ($(O),1)
O = 0
endif

ifeq ($(O),1)
DEBUG_MODE=FALSE
else
DEBUG_MODE=TRUE
endif

PKGS = gtk+-3.0 sndfile libmpg123

ifdef DEBUG_MODE
DEFINES  += -DDEBUG
CFLAGS   += -g -ggdb -D_GLIBCXX_DEBUG
#LDFLAGS  += -Wl,-no_pie
else
CFLAGS   += -O3
endif

INCLUDE  += -I $(INC_DIR)
CFLAGS   +=  -w `pkg-config --cflags $(PKGS)`
LDFLAGS  +=`pkg-config --libs $(PKGS)` -lpng -lportaudio -ldl -L./bin -l:liblua5.2.so -lboost_system
#LDFLAGS += -lgdk_imlib
CPPFLAGS += -std=c++11
GAME_NAME = thegame

GAME_OBJS = $(OBJ_DIR)/main.o                   \
    $(OBJ_DIR)/AIPlayer.o                       \
    $(OBJ_DIR)/ApplicationData.o                \
    $(OBJ_DIR)/ApplicationPath.o                \
    $(OBJ_DIR)/AssetDecoratedMap.o              \
    $(OBJ_DIR)/AssetLoader.o                    \
    $(OBJ_DIR)/AssetRenderer.o                  \
    $(OBJ_DIR)/BasicCapabilities.o              \
    $(OBJ_DIR)/BattleMode.o                     \
    $(OBJ_DIR)/Bevel.o                          \
    $(OBJ_DIR)/BuildCapabilities.o              \
    $(OBJ_DIR)/BuildingUpgradeCapabilities.o    \
    $(OBJ_DIR)/ButtonMenuMode.o                 \
    $(OBJ_DIR)/ButtonRenderer.o                 \
    $(OBJ_DIR)/CommentSkipLineDataSource.o      \
    $(OBJ_DIR)/Command.o 					   	\
    $(OBJ_DIR)/ConnectionSelectionMenuMode.o    \
    $(OBJ_DIR)/CursorSet.o                      \
    $(OBJ_DIR)/ChatDecorator.o                  \
    $(OBJ_DIR)/Debug.o                          \
    $(OBJ_DIR)/Decorator.o                      \
    $(OBJ_DIR)/EditOptionsMode.o                \
    $(OBJ_DIR)/EditRenderer.o                   \
    $(OBJ_DIR)/EndOfBattleMode.o                \
    $(OBJ_DIR)/EventHandler.o                   \
    $(OBJ_DIR)/FileDataContainer.o              \
    $(OBJ_DIR)/FileDataSink.o                   \
    $(OBJ_DIR)/FileDataSource.o                 \
    $(OBJ_DIR)/FogRenderer.o                    \
    $(OBJ_DIR)/FontTileset.o                    \
    $(OBJ_DIR)/GameLobbyMode.o                  \
    $(OBJ_DIR)/GameModel.o                      \
    $(OBJ_DIR)/GameSelectionMode.o              \
    $(OBJ_DIR)/GraphicFactoryCairo.o            \
    $(OBJ_DIR)/GraphicMulticolorTileset.o       \
    $(OBJ_DIR)/GraphicRecolorMap.o              \
    $(OBJ_DIR)/GraphicTileset.o                 \
    $(OBJ_DIR)/GUIFactoryGTK3.o                 \
    $(OBJ_DIR)/HostGameOptionsMode.o            \
    $(OBJ_DIR)/InGameMenuMode.o                 \
    $(OBJ_DIR)/IOFactoryGlib.o                  \
    $(OBJ_DIR)/LineDataSource.o                 \
    $(OBJ_DIR)/ListViewRenderer.o               \
    $(OBJ_DIR)/LogInOptionsMode.o               \
    $(OBJ_DIR)/ListDecorator.o                  \
    $(OBJ_DIR)/MapRenderer.o                    \
    $(OBJ_DIR)/MapSelectionMode.o               \
    $(OBJ_DIR)/MainMenuMode.o                   \
    $(OBJ_DIR)/MemoryDataSource.o               \
    $(OBJ_DIR)/MiniMapRenderer.o                \
	$(OBJ_DIR)/MultiplayerClient.o              \
    $(OBJ_DIR)/MultiPlayerOptionsMenuMode.o     \
    $(OBJ_DIR)/NetworkOptionsMode.o             \
    $(OBJ_DIR)/OptionsMenuMode.o                \
    $(OBJ_DIR)/Path.o                           \
    $(OBJ_DIR)/PeriodicTimeout.o                \
    $(OBJ_DIR)/PixelType.o                      \
    $(OBJ_DIR)/PlayerAIColorSelectMode.o        \
    $(OBJ_DIR)/PlayerAsset.o                    \
    $(OBJ_DIR)/Position.o                       \
    $(OBJ_DIR)/ResourceRenderer.o               \
    $(OBJ_DIR)/RouterMap.o                      \
    $(OBJ_DIR)/ServerConnectOptionMode.o        \
    $(OBJ_DIR)/SoundClip.o                      \
    $(OBJ_DIR)/SoundEventRenderer.o             \
    $(OBJ_DIR)/SoundLibraryMixer.o              \
    $(OBJ_DIR)/SoundOptionsMode.o               \
    $(OBJ_DIR)/TerrainMap.o                     \
    $(OBJ_DIR)/TextFormatter.o                  \
    $(OBJ_DIR)/Tokenizer.o                      \
    $(OBJ_DIR)/TrainCapabilities.o              \
    $(OBJ_DIR)/TriggerHandler.o                 \
    $(OBJ_DIR)/UnitActionRenderer.o             \
    $(OBJ_DIR)/UnitDescriptionRenderer.o        \
    $(OBJ_DIR)/UnitUpgradeCapabilities.o        \
    $(OBJ_DIR)/ViewportRenderer.o               \
    $(OBJ_DIR)/VisibilityMap.o

all: info directories $(BIN_DIR)/$(GAME_NAME)

$(BIN_DIR)/$(GAME_NAME): $(GAME_OBJS)
	$(Q) $(CXX) $(GAME_OBJS) -o $(BIN_DIR)/$(GAME_NAME) $(CFLAGS) $(CPPFLAGS) $(DEFINES) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(Q) $(CXX) $(CFLAGS) $(CPPFLAGS) $(DEFINES) $(INCLUDE) -c $< -o $@

.PHONY: info
info:
ifeq ($(O),0)
	@echo "Running in debug mode, please use \"make O=1\" to run in optimization mode";
else
	@echo "Running in optimization mode";
endif
ifeq ($(V),0)
	@echo "Running in silent mode, please use \"make V=1\" to run in verbose mode";
else
	@echo "Running in verbose mode";
endif



.PHONY: directories
directories:
	$(Q) mkdir -p $(OBJ_DIR)

.PHONY: clean
clean:
	$(Q) rm -f $(GAME_OBJS) $(INC_DIR)/*.*~ $(SRC_DIR)/*.*~ Debug.out