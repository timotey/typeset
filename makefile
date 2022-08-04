CC=g++
LD=g++

WARNS := all extra shadow non-virtual-dtor old-style-cast cast-align
WARNS += unused overloaded-virtual pedantic conversion sign-conversion
WARNS += misleading-indentation null-dereference double-promotion
WARNS += format=2

CFLAGS=O0 g3 std=c++20# fconcepts-diagnostics-depth=0
LFLAGS= 
LIBS=
PATH_INCS=
PATH_LIBS=
PATH_DLIB=

PATH_OBJ= build
PATH_SRC= src

BIN_NAME= build
#end of user settings

SRCS=$(shell find $(PATH_SRC) -name "*.cpp")
SRCS11=$(shell find $(PATH_SRC) -name "*.cpp11")
OBJS=$(SRCS:$(PATH_SRC)/%.cpp=$(PATH_OBJ)/%.o) $(SRCS11:$(PATH_SRC)/%.cpp11=$(PATH_OBJ)/%.o)
DEPS=$(SRCS:$(PATH_SRC)/%.cpp=$(PATH_OBJ)/%.d)

COMPILE_CPP=$(CC) -MMD -c $(patsubst %, -%,$(CFLAGS)) $(patsubst %, -I%,$(PATH_INCS)) $(patsubst %, -W%,$(WARNS))
LINK_CPP=$(LD) -o $@ $^ $(patsubst %,-rpath %,$(PATH_DLIB)) $(patsubst %, -L%, $(PATH_LIBS)) $(patsubst %, -l%, $(LIBS)) $(patsubst %, -%, $(LFLAGS))

all:$(PATH_OBJ)/$(BIN_NAME)

clean:
	rm -rf $(PATH_OBJ)

$(PATH_OBJ):
	@mkdir -p $@ 

$(PATH_OBJ)/$(BIN_NAME):$(OBJS)
	@echo "Building target" $@
	@echo "Invoking" $(LD) on $^
	@$(LINK_CPP)
	@echo "Building target" $@ "Complete"
	@chmod +x $@

$(PATH_OBJ)/%.o: $(PATH_SRC)/%.cpp11 ./makefile | $(PATH_OBJ)
	@echo "Building target" $@
	@echo "Invoking" $(CC) on $<
	@mkdir -p $(@D)
	@g++ -c -std=c++11 -x c++ -o $@ $<
	@echo "Building target" $@ "Complete"
$(PATH_OBJ)/%.o: $(PATH_SRC)/%.cpp ./makefile | $(PATH_OBJ)
	@echo "Building target" $@
	@echo "Invoking" $(CC) on $<
	@mkdir -p $(@D)
	@$(COMPILE_CPP) -o $@ $<
	@echo "Building target" $@ "Complete"

.PHONY: all clean

$(DEPS):

-include $(DEPS)
