project_root ?= $(shell while [ ! -f shared_build.mk ]; do cd ..; done ; pwd)

LIB_PREFIX_NAME := lib
OBJDIR := .objs
LIBDIR := $(project_root)/lib/
CXX=g++
CXX_VER=-std=c++20
#BOOST_INCLUDE=-isystem /usr/include/
INCLUDES = -I$(project_root) 
CXXFLAGS=-Wall -Wextra $(BOOST_INCLUDE) $(CXX_VER)

DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td
LIBDIR=$(project_root)/libs
LIB_NAME_FROM_PATH := $(shell basename `pwd`)
LIB_NAME ?= $(LIB_NAME_FROM_PATH)
SRCS := $(wildcard *.cpp)
SRCFILES_CPP := $(filter %.cpp, $(SRCS))
OBJLISTCPP := $(SRCFILES_CPP:.cpp=.o)
OBJLIST := $(OBJLISTCPP)
DEPLIST_BY_OBJ := $(OBJLIST:.o=.d)
DEPLIST_BY_OBJ1 := $(addprefix /, $(DEPLIST_BY_OBJ))
DEPLIST_FILES := $(addprefix $(DEPDIR), $(DEPLIST_BY_OBJ1))
LIB_NAME_FULL1 := $(addprefix $(LIB_PREFIX_NAME), $(LIB_NAME))
LIB_NAME_FULL := $(LIB_NAME_FULL1).a
LIB ?= $(addprefix $(LIBDIR)/, $(LIB_NAME_FULL))
OBJS := $(addprefix $(OBJDIR)/, $(OBJLIST))

COMPILE.cc = @$(CXX) $(DEPFLAGS) $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(CFLAGS4CPP) $(INCLUDES) $(TARGET_ARCH) -c
POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d

$(OBJDIR)/%.o : %.cpp
$(OBJDIR)/%.o : %.cpp $(DEPDIR)/%.d
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)
	@echo "finish compiling $<"

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

$(OBJDIR) : | $(DEPDIR)
	@mkdir -p $(OBJDIR)

$(DEPDIR) :
	@mkdir -p $(DEPDIR)

-include $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRCS)))

.PHONY: all $(LIB)
.PHONY: build_lib $(LIB)
.PHONY: clean

all: $(LIB)
build_lib: $(LIB)
	@echo "building library $(LIB) is done"

define create_static_lib        
	@$(AR) rc $(LIB) $(OBJS)
endef

$(OBJS) : | $(OBJDIR)

#@echo "objs list is $(OBJS) objs list $(OBJLIST) SRCS = $(SRCS) "
$(LIB): $(OBJS) | $(LIBDIR)
	@echo "finish building the objects, going to build make a lib"
	$(create_static_lib)
        
$(LIBDIR): 
	@mkdir -p $(LIBDIR)
        
clean:
	@rm -rf $(DEPDIR) $(OBJDIR) $(LIB)
	@echo "finish cleaning $(LIB)"
