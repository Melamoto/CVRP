CC=g++
CFLAGS= -std=c++11 -Wall -O3
OBJS= savings.o cvrp.o cvrpSolver.o tabu.o

all: cvrpSolver

cvrpSolver: $(OBJS)
	@${CC} ${CFLAGS} -o ${@} $^

%.o    : %.cpp
	@${CC} ${CFLAGS} -c -o ${@} $<

clean:
	@rm -f cvrpSolver $(OBJS)
