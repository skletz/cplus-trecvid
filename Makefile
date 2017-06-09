# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#	Makefile for vretbox (Video Retrieval Evaluation Toolbox)
# @author skletz
# @version 1.0, 08/06/17
# -----------------------------------------------------------------------------
# CMD Arguments:	os=win,linux (sets the operating system, default=linux)
#									opencv=usr,opt (usr=/user/local/, opt=/opt/local/; default=usr)
# -----------------------------------------------------------------------------
# @TODO: Make for Windows (currently the option is only considered)
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

# Dependencies
DEPEND = ./libs/cplusutil ./libs/cpluslogger ./libs/opencv-histlib ./libs/opencv-pctsig ./libs/opencv-tfsig ./libs/defuse

.PHONY: all

all: directories libraries copy prog

directories:
	mkdir -p $(BUILD)/$(BIN)
	mkdir -p $(BUILD)/$(LIB)
	mkdir -p $(BUILD)/$(EXT)

prog: $(OBJECTS)
		$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(BUILD)/$(BIN)/prog$(PROJECT).$(VERSION) $(LDLIBSOPTIONS)

$(BUILD)/$(EXT)/%.o: $(SRC)/%.cpp
		$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@ $(INCDIR)

# make dependencies
libraries:
	for i in $(DEPEND); do \
		echo ============================================================================== ; \
		echo === working in $$i ; \
		echo ============================================================================== ; \
		cd $$i; \
		make; \
	done

copy:
	for i in $(DEPEND); do \
	echo ============================================================================== ; \
	echo === copying $$i ; \
	echo ============================================================================== ; \
		cp -f $$i/$(BUILD)/$(BIN)/* ./$(BUILD)/$(BIN); \
		cp -f $$i/$(BUILD)/$(LIB)/* ./$(BUILD)/$(LIB); \
		cp -f $$i/$(BUILD)/$(EXT)/* ./$(BUILD)/$(EXT); \
	done

logger:
	echo "CPLUSLogger: " $(LOGGER)
	$(MAKE) -C $(LOGGER)/ all

util:
	echo "CPLUSUtil: " $(UTIL)
	$(MAKE) -C $(UTIL)/ all

pctsignatures:
	echo "Position-Color-Texture Signatures: " $(PCTSIGNATURES)
	$(MAKE) -C $(PCTSIGNATURES)/ os=$(os) opencv=$(opencv) all

tfsignatuers:
	echo "Track-based Signatures: " $(TFSIGNATURES)
	$(MAKE) -C $(TFSIGNATURES)/ os=$(os) opencv=$(opencv) all

histlib:
	echo "OpenCV-HistLib: " $(CVHISTLIB)
	$(MAKE) -C $(CVHISTLIB)/ os=$(os) opencv=$(opencv) all

defuse:
	echo "DeFUSE: " $(DEFUSEDIR)
	$(MAKE) -C $(DEFUSEDIR)/ os=$(os) opencv=$(opencv) all

clean:
	rm -rf $(BUILD)

info:
	@echo "OpenCV Installation: " $(OPENCVDIR) "\n"
	@echo "OpenCV Header Include Directory: " $(INCDIR) "\n"
	@echo "OpenCV Linking: " $(LDLIBSOPTIONS) "\n"
