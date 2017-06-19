# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#	Makefile for vretbox (Video Retrieval Evaluation Toolbox)
#
# @author skletz
# @version 1.1, 19/06/17 bug fix make all
# @version 1.0, 08/06/17
# -----------------------------------------------------------------------------
# CMD Arguments:	os=win,linux (sets the operating system, default=linux)
#									opencv=usr,opt (usr=/user/local/, opt=/opt/local/; default=usr)
# -----------------------------------------------------------------------------
# @TODO: Make for Windows (currently only option is considered)
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

# Default command line arguments
os = linux
opencv = usr

# Compiler, flags
CXX=g++
CXXFLAGS=-std=c++11 -m64

# Output directory
BUILD =	builds
BIN =	linux/bin
LIB =	linux/lib
EXT =	linux/ext

# Project settings
PROJECT=vretbox
VERSION=1.0

SRC= $(PROJECT)/src
SOURCES=$(wildcard $(SRC)/*.cpp)
OBJECTS=$(patsubst $(SRC)/%.cpp,$(BUILD)/$(EXT)/%.o,$(SOURCES))

# Default settings for opencv installation directory
OPENCVDIR := /usr/local/
INCDIR = `pkg-config --cflags opencv`
LDLIBSOPTIONS_POST =`pkg-config --libs opencv`
LDLIBSOPTIONS_POST += -L/usr/local/share/OpenCV/3rdparty/lib

# operating system can be changed via command line argument
ifeq ($(os),win)
	BIN := win/bin
	LIB := win/lib
	EXT := win/ext
endif

# opencv installation dir can be changed via command line argument
ifeq ($(opencv),opt)
	OPENCVDIR := /opt/local/
	INCDIR = `pkg-config --cflags /opt/local/lib/pkgconfig/opencv.pc`
	LDLIBSOPTIONS_POST =`pkg-config --libs /opt/local/lib/pkgconfig/opencv.pc`
endif

# OpenCV, Boost Cplusutil, Cplulogger Directory, Header Files, Libraries
# Staic Feature Signatures
PCTSIGNATURES="./libs/opencv-pctsig"
# Track-based Feature Signatures
TFSIGNATURES="./libs/opencv-tfsig"
# Visualization of motion histogram
CVHISTLIB="./libs/opencv-histlib"
UTIL="./libs/cplusutil"
LOGGER="./libs/cpluslogger"
#Dynamic Features
DEFUSE="./libs/defuse"

# contains all compiled libraries
LIBS="$(BUILD)/$(LIB)"

LDLIBSOPTIONS = $(LIBS)/libcpluslogger.1.0.a
LDLIBSOPTIONS += $(LIBS)/libdefuse.2.0.a
LDLIBSOPTIONS += $(LIBS)/libcplusutil.1.0.a
LDLIBSOPTIONS += $(LIBS)/libcvhistlib.1.0.a
LDLIBSOPTIONS += $(LIBS)/libcvtfsig.1.0.a
LDLIBSOPTIONS += $(LIBS)/libcvpctsig.1.0.a

LDLIBSOPTIONS += -lboost_filesystem -lboost_system -lboost_serialization -lboost_program_options
#IMPORTANT: Link sequence! - OpenCV libraries have to be added at the end
LDLIBSOPTIONS += $(LDLIBSOPTIONS_POST)

INCDIR+= -I$(LOGGER)/cpluslogger/include
INCDIR+= -I$(UTIL)/cplusutil/include
INCDIR+= -I$(PCTSIGNATURES)/cvpctsig/include
INCDIR+= -I$(TFSIGNATURES)/cvtfsig/include
INCDIR+= -I$(CVHISTLIB)/include
INCDIR+= -I$(DEFUSE)/defuse/include

UTIL="./libs/cplusutil"
LOGGER="./libs/cpluslogger"
CVHISTLIB="./libs/opencv-histlib"
PCTSIGNATURES="./libs/opencv-pctsig"
TFSIGNATURES="./libs/opencv-tfsig"
DEFUSEDIR="./libs/defuse"

.PHONY: all

all: clean directories libraries prog

directories:
	@echo "==============================================================================" ;
	@echo "Making Directories"
	@echo "==============================================================================" ;
	mkdir -p $(BUILD)/$(BIN)
	mkdir -p $(BUILD)/$(LIB)
	mkdir -p $(BUILD)/$(EXT)

prog: obj
	@echo "==============================================================================" ;
	@echo "Making Program"
	@echo "==============================================================================" ;

obj: $(OBJECTS)
			$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(BUILD)/$(BIN)/prog$(PROJECT).$(VERSION) $(LDLIBSOPTIONS)

$(BUILD)/$(EXT)/%.o: $(SRC)/%.cpp
		$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@ $(INCDIR)

# make dependencies
libraries: logger util pctsignatures tfsignatuers histlib defuse

logger:
	@echo "==============================================================================" ;
	@echo "Making CPLUSLogger: " $(LOGGER)
	@echo "==============================================================================" ;
	$(MAKE) -C $(LOGGER)/ clean
	$(MAKE) -C $(LOGGER)/ all
	@echo "==============================================================================" ;
	@echo "Copying CPLUSLogger: " $(LOGGER)
	@echo "==============================================================================" ;
	cp -f $(LOGGER)/$(BUILD)/$(BIN)/* ./$(BUILD)/$(BIN);
	cp -f $(LOGGER)/$(BUILD)/$(LIB)/* ./$(BUILD)/$(LIB);
	cp -f $(LOGGER)/$(BUILD)/$(EXT)/* ./$(BUILD)/$(EXT);

util:
	@echo "==============================================================================" ;
	@echo "Making CPLUSUtil: " $(UTIL)
	@echo "==============================================================================" ;
	$(MAKE) -C $(UTIL)/ clean
	$(MAKE) -C $(UTIL)/ all
	@echo "==============================================================================" ;
	@echo "Copying CPLUSUtil: " $(UTIL)
	@echo "==============================================================================" ;
	cp -f $(UTIL)/$(BUILD)/$(BIN)/* ./$(BUILD)/$(BIN);
	cp -f $(UTIL)/$(BUILD)/$(LIB)/* ./$(BUILD)/$(LIB);
	cp -f $(UTIL)/$(BUILD)/$(EXT)/* ./$(BUILD)/$(EXT);

pctsignatures:
	@echo "==============================================================================" ;
	@echo "Making Position-Color-Texture Signatures: " $(PCTSIGNATURES)
	@echo "==============================================================================" ;
	$(MAKE) -C $(PCTSIGNATURES)/ os=$(os) opencv=$(opencv) clean
	$(MAKE) -C $(PCTSIGNATURES)/ os=$(os) opencv=$(opencv) all
	@echo "==============================================================================" ;
	@echo "Copying Position-Color-Texture Signatures: " $(PCTSIGNATURES)
	@echo "==============================================================================" ;
	cp -f $(PCTSIGNATURES)/$(BUILD)/$(BIN)/* ./$(BUILD)/$(BIN);
	cp -f $(PCTSIGNATURES)/$(BUILD)/$(LIB)/* ./$(BUILD)/$(LIB);
	cp -f $(PCTSIGNATURES)/$(BUILD)/$(EXT)/* ./$(BUILD)/$(EXT);

tfsignatuers:
	@echo "==============================================================================" ;
	@echo "Making Track-based Signatures: " $(TFSIGNATURES)
	@echo "==============================================================================" ;
	$(MAKE) -C $(TFSIGNATURES)/ os=$(os) opencv=$(opencv) clean
	$(MAKE) -C $(TFSIGNATURES)/ os=$(os) opencv=$(opencv) all
	@echo "==============================================================================" ;
	@echo "Copying Track-based Signatures: " $(TFSIGNATURES)
	@echo "==============================================================================" ;
	cp -f $(TFSIGNATURES)/$(BUILD)/$(BIN)/* ./$(BUILD)/$(BIN);
	cp -f $(TFSIGNATURES)/$(BUILD)/$(LIB)/* ./$(BUILD)/$(LIB);
	cp -f $(TFSIGNATURES)/$(BUILD)/$(EXT)/* ./$(BUILD)/$(EXT);

histlib:
	@echo "==============================================================================" ;
	@echo "Making OpenCV-HistLib: " $(CVHISTLIB)
	@echo "==============================================================================" ;
	$(MAKE) -C $(CVHISTLIB)/ os=$(os) opencv=$(opencv) clean
	$(MAKE) -C $(CVHISTLIB)/ os=$(os) opencv=$(opencv) all
	@echo "==============================================================================" ;
	@echo "Copying OpenCV-HistLib: " $(CVHISTLIB)
	@echo "==============================================================================" ;
	cp -f $(CVHISTLIB)/$(BUILD)/$(BIN)/* ./$(BUILD)/$(BIN);
	cp -f $(CVHISTLIB)/$(BUILD)/$(LIB)/* ./$(BUILD)/$(LIB);
	cp -f $(CVHISTLIB)/$(BUILD)/$(EXT)/* ./$(BUILD)/$(EXT);

defuse:
	@echo "==============================================================================" ;
	@echo "Making DeFUSE: " $(DEFUSEDIR)
	@echo "==============================================================================" ;
	$(MAKE) -C $(DEFUSEDIR)/ os=$(os) opencv=$(opencv) clean
	$(MAKE) -C $(DEFUSEDIR)/ os=$(os) opencv=$(opencv) all
	@echo "==============================================================================" ;
	@echo "Copying DeFUSE: " $(DEFUSEDIR)
	@echo "==============================================================================" ;
	cp -f $(DEFUSEDIR)/$(BUILD)/$(BIN)/* ./$(BUILD)/$(BIN);
	cp -f $(DEFUSEDIR)/$(BUILD)/$(LIB)/* ./$(BUILD)/$(LIB);
	cp -f $(DEFUSEDIR)/$(BUILD)/$(EXT)/* ./$(BUILD)/$(EXT);

clean:
	rm -rf $(BUILD)

info:
	@echo "OpenCV Installation: " $(OPENCVDIR) "\n"
	@echo "OpenCV Header Include Directory: " $(INCDIR) "\n"
	@echo "OpenCV Linking: " $(LDLIBSOPTIONS) "\n"
