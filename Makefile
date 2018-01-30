#rule
#target: pre-requisites
#tab recipe
#tab

OBJ = main.o enigma.o
EXE = enigma
CXX = g++
CXXFLAGS = -Wall -g -std=c++11 

$(EXE): $(OBJ)
	$(CXX) $(OBJ) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

main.o: enigma.h

enigma.o: enigma.h

clean:
	rm -f $(OBJ) $(EXE)

.PHONY: clean
