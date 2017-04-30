CXXFLAGS:= -mavx
override CXXFLAGS+= -std=c++1y -ffast-math -ggdb3 -Wall
OBJS:= main.o
override LIBS+= -lboost_program_options

ds3-armor: $(OBJS)
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -o ds3-armor $(OBJS) $(LIBS)

release: CXXFLAGS+= -O3
release: LDFLAGS+= -static
release: ds3-armor
debug: ds3-armor
sanitize: CXXFLAGS+= -fsanitize=address,undefined -O3
sanitize: ds3-armor

.DEFAULT_GOAL := release

clean:
	rm -f ds3-armor $(OBJS)

makedepend:
	makedepend -a -Y $(OBJS:.o=.cpp)

.PHONY: clean makedepend release debug sanitize
