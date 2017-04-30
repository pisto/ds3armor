CXXFLAGS:= -march=native -mtune=native
override CXXFLAGS+= -std=c++1y -ffast-math -Wall
OBJS:= main.o
override LIBS+= -lboost_program_options

ds3-armor: $(OBJS)
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -o ds3-armor $(OBJS) $(LIBS)

release: CXXFLAGS+= -O3
release: ds3-armor
debug: CXXFLAGS+= -ggdb3
debug: ds3-armor
sanitize: CXXFLAGS+= -fsanitize=address,undefined -O3 -ggdb3
sanitize: ds3-armor

.DEFAULT_GOAL := release

clean:
	rm -f ds3-armor $(OBJS)

makedepend:
	makedepend -a -Y $(OBJS:.o=.cpp)

.PHONY: clean makedepend release debug sanitize
