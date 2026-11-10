PLATFORM=$(shell uname)
ENDIANESS=OSC_HOST_LITTLE_ENDIAN

LD_FLAGS = -lGL -lGLU -lglut

TUIO_DEMO = Tuio2Demo
TUIO_DUMP = Tuio2Dump
TUIO_SIMULATOR = Tuio2Simulator
TUIO_TEST = Tuio2Test
TUIO_STATIC  = libTUIO2.a
TUIO_SHARED  = libTUIO2.so

SDL_LDFLAGS := $(shell sdl2-config --libs 2>/dev/null)
SDL_CFLAGS  := $(shell sdl2-config --cflags 2>/dev/null)

INCLUDES = -I../TUIO -I../TUIO2 -I../oscpack
#CFLAGS  = -g -Wall -O3 -fPIC $(SDL_CFLAGS)
CFLAGS  = -w -O3 -fPIC $(SDL_CFLAGS)
CXXFLAGS = $(CFLAGS) $(INCLUDES) -D$(ENDIANESS)
SHARED_OPTIONS = -shared -Wl,-soname,$(TUIO_SHARED)

ifeq ($(PLATFORM), Darwin)
	TARGET = -mmacosx-version-min=11.0 -arch arm64 -arch x86_64
	CFLAGS += $(TARGET) -DGL_SILENCE_DEPRECATION
	CXXFLAGS += $(TARGET)
	TUIO_SHARED = libTUIO2.dylib
	LD_FLAGS = -framework OpenGL -framework GLUT -framework SDL2 -framework Cocoa
	SHARED_OPTIONS = -dynamiclib -Wl,-install_name,@rpath/$(TUIO_SHARED)
	SDL_LDFLAGS = -F/Library/Frameworks -Wl,-rpath,/Library/Frameworks
	SDL_CFLAGS = -F/Library/Frameworks
	DEMO_PLIST = -Wl,-sectcreate,__TEXT,__info_plist,Tuio2Demo.plist
	SIMULATOR_PLIST = -Wl,-sectcreate,__TEXT,__info_plist,Tuio2Simulator.plist
endif

%.o: %.cpp
	@echo [CXX] $@
	@ $(CXX) $(CXXFLAGS) -o $@ -c $<

DEMO_SOURCES = Tuio2Demo.cpp
DEMO_OBJECTS = Tuio2Demo.o
DUMP_SOURCES = Tuio2Dump.cpp
DUMP_OBJECTS = Tuio2Dump.o
TEST_SOURCES = Tuio2Test.cpp
TEST_OBJECTS = Tuio2Test.o
SIMULATOR_SOURCES = Tuio2Simulator.cpp
SIMULATOR_OBJECTS = Tuio2Simulator.o

COMMON_TUIO_SOURCES = ../TUIO/TuioTime.cpp ../TUIO/TuioPoint.cpp ../TUIO/OneEuroFilter.cpp ../TUIO2/TuioObject.cpp ../TUIO2/TuioComponent.cpp ../TUIO2/TuioToken.cpp ../TUIO2/TuioPointer.cpp ../TUIO2/TuioBounds.cpp ../TUIO2/TuioSymbol.cpp ../TUIO2/TuioGeometry.cpp ../TUIO2/TuioContent.cpp ../TUIO2/TuioControl.cpp ../TUIO2/TuioData.cpp ../TUIO2/TuioSignal.cpp ../TUIO2/TuioComponent3D.cpp ../TUIO2/TuioToken3D.cpp ../TUIO2/TuioPointer3D.cpp ../TUIO2/TuioBounds3D.cpp ../TUIO2/TuioDispatcher.cpp ../TUIO2/TuioManager.cpp 
SERVER_TUIO_SOURCES = ../TUIO2/TuioServer.cpp ../TUIO/UdpSender.cpp ../TUIO/TcpSender.cpp ../TUIO/WebSockSender.cpp
CLIENT_TUIO_SOURCES = ../TUIO2/TuioClient.cpp ../TUIO/OscReceiver.cpp ../TUIO/UdpReceiver.cpp ../TUIO/TcpReceiver.cpp
OSC_SOURCES = ../oscpack/osc/OscTypes.cpp ../oscpack/osc/OscOutboundPacketStream.cpp ../oscpack/osc/OscReceivedElements.cpp ../oscpack/osc/OscPrintReceivedElements.cpp ../oscpack/ip/posix/NetworkingUtils.cpp ../oscpack/ip/posix/UdpSocket.cpp

COMMON_TUIO_OBJECTS = $(COMMON_TUIO_SOURCES:.cpp=.o)
SERVER_TUIO_OBJECTS = $(SERVER_TUIO_SOURCES:.cpp=.o)
CLIENT_TUIO_OBJECTS = $(CLIENT_TUIO_SOURCES:.cpp=.o)
OSC_OBJECTS = $(OSC_SOURCES:.cpp=.o)

TEST_SUITE_SOURCES = $(wildcard ../tests/*.cpp)
TEST_SUITE_OBJECTS = $(TEST_SUITE_SOURCES:.cpp=.o)
RUN_TESTS = runTests

.PHONY: all clean test

test: $(RUN_TESTS)
	./$(RUN_TESTS)

all: $(TUIO_DUMP) $(TUIO_DEMO) $(TUIO_SIMULATOR) $(TUIO_TEST) $(TUIO_STATIC) $(TUIO_SHARED)

$(TUIO_STATIC): $(COMMON_TUIO_OBJECTS) $(CLIENT_TUIO_OBJECTS) $(SERVER_TUIO_OBJECTS) $(OSC_OBJECTS)
	@echo [LD] $(TUIO_STATIC)
	@ rm -f $@
ifeq ($(PLATFORM), Darwin)
	@ libtool -static -o $@ $+
else
	@ ar rcs $@ $+
endif

$(TUIO_SHARED): $(COMMON_TUIO_OBJECTS) $(CLIENT_TUIO_OBJECTS) $(SERVER_TUIO_OBJECTS) $(OSC_OBJECTS)
	@echo [LD] $(TUIO_SHARED)
	@ $(CXX) -o $@ $+ -lpthread $(TARGET) $(SHARED_OPTIONS)

$(TUIO_DUMP): $(COMMON_TUIO_OBJECTS) $(CLIENT_TUIO_OBJECTS) $(OSC_OBJECTS) $(DUMP_OBJECTS)
	@echo [LD] $(TUIO_DUMP)
	@ $(CXX) -o $@ $+ -lpthread $(TARGET)

$(TUIO_DEMO): $(COMMON_TUIO_OBJECTS) $(CLIENT_TUIO_OBJECTS) $(OSC_OBJECTS) $(DEMO_OBJECTS)
	@echo [LD] $(TUIO_DEMO)
	@ $(CXX) -o $@ $+ -lpthread $(TARGET) $(SDL_LDFLAGS) $(LD_FLAGS) $(DEMO_PLIST)

$(TUIO_SIMULATOR): $(COMMON_TUIO_OBJECTS) $(SERVER_TUIO_OBJECTS) $(OSC_OBJECTS) $(SIMULATOR_OBJECTS)
	@echo [LD] $(TUIO_SIMULATOR)
	@ $(CXX) -o $@ $+ -lpthread $(TARGET) $(SDL_LDFLAGS) $(LD_FLAGS) $(SIMULATOR_PLIST)

$(TUIO_TEST): $(COMMON_TUIO_OBJECTS) $(SERVER_TUIO_OBJECTS) $(OSC_OBJECTS) $(TEST_OBJECTS)
	@echo [LD] $(TUIO_TEST)
	@ $(CXX) -o $@ $+ -lpthread $(TARGET)

$(RUN_TESTS): $(COMMON_TUIO_OBJECTS) $(CLIENT_TUIO_OBJECTS) $(SERVER_TUIO_OBJECTS) $(OSC_OBJECTS) $(TEST_SUITE_OBJECTS)
	@echo [LD] $(RUN_TESTS)
	@ $(CXX) -o $@ $+ -lpthread $(TARGET)

clean:
	@echo [CLEAN] $(TUIO_DUMP) $(TUIO_DEMO) $(TUIO_SIMULATOR) $(TUIO_TEST) $(TUIO_STATIC) $(TUIO_SHARED)
	@ rm -f $(TUIO_DUMP) $(TUIO_DEMO) $(TUIO_SIMULATOR) $(TUIO_TEST) $(TUIO_STATIC) $(TUIO_SHARED) $(RUN_TESTS)
	@ rm -f $(COMMON_TUIO_OBJECTS) $(CLIENT_TUIO_OBJECTS) $(SERVER_TUIO_OBJECTS) $(TEST_OBJECTS) $(OSC_OBJECTS) $(DUMP_OBJECTS) $(DEMO_OBJECTS) $(SIMULATOR_OBJECTS) $(TEST_SUITE_OBJECTS)
