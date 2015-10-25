# Build targets
Q1_O=./query1.cpp
Q2_O=./query2.cpp
Q12_O=./query12.cpp
Q3_O=./query3.cpp
Q4_O=./query4.cpp
Q4_alg=./query4_algorithm.cpp

PRE_O = splitfile.cpp
POST_O=mergeans.cpp

# Compiler Flags

USER_FLAGS=

CC=gcc
CXX=g++
CFLAGS=$(USER_FLAGS) -O3 -Wall
CXXFLAGS=$(CFLAGS)

# Objective file names
Query1=query1
Query2=query2
Query12=query12
Query3=query3
Query4=query4
Pre=splitfile
Post=mergeans

all: $(Query1) $(Query2) $(Query3) $(Query4) $(Pre) $(Post) 

$(Query1): 
	$(CXX) $(CXXFLAGS) -o $(Query1) $(Q1_O)

$(Query2): 
	$(CXX) $(CXXFLAGS) -o $(Query2) $(Q2_O)

#$(Query12):
#	$(CXX) $(CXXFLAGS) -o $(Query12) $(Q12_O)

$(Query3): 
	$(CXX) $(CXXFLAGS) -o $(Query3) $(Q3_O)

$(Query4): 
	$(CXX) $(CXXFLAGS) -o $(Query4) $(Q4_O) $(Q4_alg)

$(Pre): 
	$(CXX) $(CXXFLAGS) -o $(Pre) $(PRE_O)

$(Post): 
	$(CXX) $(CXXFLAGS) -o $(Post) $(POST_O)

clean:
	rm -f $(Query1) $(Query2) $(Query3) $(Query4) $(Pre) $(Post)
	find . -name '*.o' -print | xargs rm -f

