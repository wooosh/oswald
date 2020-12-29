BUILDDIR = build
TARGET = oswald
SRC = $(wildcard src/*.cxx)
OBJ = $(patsubst src/%.cxx,build/%.o,$(SRC))
DEP = $(OBJ:.o=.d)

CXXFLAGS += -std=c++17 -O3 -Iinclude

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $?

-include $(DEP)

build/%.o: src/%.cxx | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -MMD -MP -o $@ -c $<

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	$(RM) -r $(BUILDDIR)
	$(RM) $(TARGET)
