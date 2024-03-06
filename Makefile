CXX=    g++
EMXX=   em++
MKDIR=  mkdir
RM=     rm -f
RMDIR=  rm -fd

JS_DIR=         js/
SRCS_DIR=       srcs/
INC_DIR=        srcs/
OBJS_DIR=       objs/
BIN_DIR=        bin/
NATIVE_DIR=     native/
WEBASM_DIR=     webasm/
RELEASE_DIR=    release/
DEBUG_DIR=      debug/


OBJS_NATIVE_DIR=        $(OBJS_DIR)$(NATIVE_DIR)
OBJS_WEBASM_DIR=        $(OBJS_DIR)$(WEBASM_DIR)
NATIVE_BIN_DIR=         $(BIN_DIR)
WEBASM_BIN_DIR=         $(JS_DIR)$(BIN_DIR)

OBJS_DIRS=  $(OBJS_NATIVE_DIR)  \
            $(OBJS_WEBASM_DIR)  \
            $(OBJS_DIR)         \

BIN_DIRS=       $(NATIVE_BIN_DIR)   \
                $(WEBASM_BIN_DIR)   \



CXX_FLAGS=  -Wall -Werror   \
            -std=c++17      \
            -I $(INC_DIR)   \
            -O3

LD_NATIVE_FLAGS= -O3 -lSDL2 -lGL -lGLEW

LD_WEBASM_FLAGS= -O3                     \
                 -s WASM=1               \
                 -s USE_SDL=2            \
                 -s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=0  \
                 -s MIN_WEBGL_VERSION=2  \
                 -s MAX_WEBGL_VERSION=2  \
                 -s FULL_ES3=1           \
                 --preload-files=shaders/\


SRCS_FILES=     main.cpp          \
                Application.cpp   \
                Clock.cpp         \
                Config.cpp        \
                FrameBuffer.cpp   \
                ShaderProgram.cpp \
                Texture.cpp       \
                Window.cpp        \


SRCS=       $(addprefix $(SRCS_DIR),$(SRCS_FILES))

OBJS_NATIVE=    $(SRCS:$(SRCS_DIR)%.cpp=$(OBJS_NATIVE_DIR)%.o)
OBJS_WEBASM=    $(SRCS:$(SRCS_DIR)%.cpp=$(OBJS_WEBASM_DIR)%.o)
OBJS=           $(OBJS_NATIVE)  \
                $(OBJS_WEBASM)  \

DEPS=           $(OBJS:.o=.dep)

NATIVE_NAME=    lenia-toybox
BIN_NATIVE=     $(NATIVE_BIN_DIR)$(NATIVE_NAME)
BIN_WEBASM=     $(WEBASM_BIN_DIR)$(NATIVE_NAME).js
RESIDUE_WEBASM= $(BIN_WEBASM:%.js=%.worker.js) $(BIN_WEBASM:%.js=%.wasm) $(BIN_WEBASM:%.js=%.data)
BINS=           $(BIN_NATIVE) \
                $(BIN_WEBASM) \

RESIDUE_BINS=       $(RESIDUE_WEBASM)



.SECONDEXPANSION:

include $(shell test -d $(OBJS_DIR) && find $(OBJS_DIR) -name "*.dep")

%/:
	$(MKDIR) -p $@


$(OBJS_NATIVE_DIR): | $$(dir $$(@D))

$(OBJS_NATIVE_DIR)%.o:  $(SRCS_DIR)%.cpp | $$(@D)/
	$(CXX) $(CXX_FLAGS) $(CXX_NATIVE_FLAGS) -MMD -MF $(@:.o=.dep) -c $< -o $@


$(OBJS_WEBASM_DIR):     | $$(dir $$(@D))

$(OBJS_WEBASM_DIR)%.o:  $(SRCS_DIR)%.cpp | $$(@D)/
	$(EMXX)  $(CXX_FLAGS) $(CXX_WEBASM_FLAGS) -MMD -MF $(@:.o=.dep) -c $< -o $@


$(BIN_NATIVE):  $(OBJS_NATIVE) | $$(@D)/
	$(CXX) $(OBJS_NATIVE) -o $(BIN_NATIVE) $(LD_NATIVE_FLAGS)


$(BIN_WEBASM):  $(OBJS_WEBASM) | $$(@D)/
	$(EMXX) $(OBJS_WEBASM) -o $(BIN_WEBASM) $(LD_WEBASM_FLAGS)


all:    $(BINS)

clean:
	$(RM) $(OBJS) $(DEPS)
	$(RMDIR) $(OBJS_DIRS)

fclean:
	$(RM) $(OBJS) $(DEPS)
	$(RMDIR) $(OBJS_DIRS)
	$(RM) $(BINS)
	$(RM) $(RESIDUE_BINS)
	$(RMDIR) $(BIN_DIRS)

re: fclean native

# aliases command
native:     $(BIN_NATIVE)
webasm:     $(BIN_WEBASM)

.DEFAULT_GOAL=  native
