CXX = g++
CXXFLAGS = -O -std=c++20 -Wall -I ../lexertl14/include -I ../parsertl14/include

LDFLAGS = -O

LIBS = 

all: idl2cpp

idl2cpp: output.o output_enums.o main.o output_events_header.o\
 output_events_source.o output_fwd_decl.o output_header.o \
 output_source.o parser.o predefined.o structs.o
	$(CXX) $(LDFLAGS) -o idl2cpp output.o output_enums.o main.o output_events_header.o\
 output_events_source.o output_fwd_decl.o output_header.o \
 output_source.o parser.o predefined.o structs.o $(LIBS)

output.o: output.cpp
	$(CXX) $(CXXFLAGS) -o output.o -c output.cpp

output_enums.o: output_enums.cpp
	$(CXX) $(CXXFLAGS) -o output_enums.o -c output_enums.cpp

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -o main.o -c main.cpp

output_events_header.o: output_events_header.cpp
	$(CXX) $(CXXFLAGS) -o output_events_header.o -c output_events_header.cpp

output_events_source.o: output_events_source.cpp
	$(CXX) $(CXXFLAGS) -o output_events_source.o -c output_events_source.cpp

output_fwd_decl.o: output_fwd_decl.cpp
	$(CXX) $(CXXFLAGS) -o output_fwd_decl.o -c output_fwd_decl.cpp

output_header.o: output_header.cpp
	$(CXX) $(CXXFLAGS) -o output_header.o -c output_header.cpp

output_source.o: output_source.cpp
	$(CXX) $(CXXFLAGS) -o output_source.o -c output_source.cpp

parser.o: parser.cpp
	$(CXX) $(CXXFLAGS) -o parser.o -c parser.cpp

predefined.o: predefined.cpp
	$(CXX) $(CXXFLAGS) -o predefined.o -c predefined.cpp

structs.o: structs.cpp
	$(CXX) $(CXXFLAGS) -o structs.o -c structs.cpp

library:

binary:

clean:
	- rm *.o
	- rm idl2cpp

