TARGET = vectest
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -ansi
ARCH = $(shell $(CC) -dumpmachine | cut -d - -f1)

LIBSDIR = libs
OBJSDIR = obj

MAJOR_VERSION = $(shell grep -Eo '.*MAJOR_VERSION  *[0-9]+' \
	version/version.h | grep -o '[0-9]*')
MINOR_VERSION = $(shell grep -Eo '.*MINOR_VERSION  *[0-9]+' \
	version/version.h | grep -o '[0-9]*')
PATCH_VERSION = $(shell grep -Eo '.*PATCH_VERSION  *[0-9]+' \
	version/version.h | grep -o '[0-9]*')
BUILD_VERSION = $(shell grep -Eo '.*BUILD_VERSION  *[0-9]+' \
	version/version.h | grep -o '[0-9]*')

ifeq ($(OS),Windows_NT)
	SYSNAME = Windows
else
	UNAMEOS = $(shell uname -s)
	ifeq ($(UNAMEOS),Darwin)
		SYSNAME = macOS
	endif
	ifeq ($(UNAMEOS),Linux)
		SYSNAME = Linux
	endif
endif

ifeq ($(DEBUG), yes)
	BASE_LIB_NAME = libcvectord
	RCSDEBUG = -DDEBUG
	LIBDIR = libs/debug
	STATIC_LIBNAME = $(BASE_LIB_NAME).a
	ifeq ($(SYSNAME),Windows)
		SHARED_LIBNAME = $(BASE_LIB_NAME).dll
		SHARED_LIB_VER_NAME = $(SHARED_LIBNAME)
	else ifeq ($(SYSNAME),macOS)
		SHARED_LIBNAME = $(BASE_LIB_NAME).dylib
		SHARED_LIB_VER_NAME = $(BASE_LIB_NAME).$(MAJOR_VERSION).$(MINOR_VERSION).$(PATCH_VERSION).dylib
	else	
		SHARED_LIBNAME = $(BASE_LIB_NAME).so
		SHARED_LIB_VER_NAME = $(BASE_LIB_NAME).so.$(MAJOR_VERSION).$(MINOR_VERSION).$(PATCH_VERSION)
	endif
	LIBPOSTFIX = d
	CFLAGS += -g

	OBJDIR = obj/debug
else
	BASE_LIB_NAME = libcvector
	LIBDIR = libs/release
	STATIC_LIBNAME = $(BASE_LIB_NAME).a
	ifeq ($(OS),Windows_NT)
		SHARED_LIBNAME = $(BASE_LIB_NAME).dll
		SHARED_LIB_VER_NAME = $(SHARED_LIBNAME)
	else ifeq ($(SYSNAME),macOS)
		SHARED_LIBNAME = $(BASE_LIB_NAME).dylib
		SHARED_LIB_VER_NAME = $(BASE_LIB_NAME).$(MAJOR_VERSION).$(MINOR_VERSION).$(PATCH_VERSION).dylib
	else	
		SHARED_LIBNAME = $(BASE_LIB_NAME).so
		SHARED_LIB_VER_NAME = $(BASE_LIB_NAME).so.$(MAJOR_VERSION).$(MINOR_VERSION).$(PATCH_VERSION)
	endif
	OBJDIR = obj/release
	CFLAGS += -Os -s
endif

STATIC_LIB_TARGET = $(LIBDIR)/static/$(STATIC_LIBNAME)
SHARED_LIB_TARGET = $(LIBDIR)/shared/$(SHARED_LIB_VER_NAME)
STATIC_OBJDIR = $(OBJDIR)/cvector/static
SHARED_OBJDIR = $(OBJDIR)/cvector/shared

ifeq ($(SYSNAME),Windows)
	RCS = $(wildcard version/*.rc)
	RES = $(patsubst %.rc, $(OBJDIR)/%.res, $(RCS))
	SHARED_LIB_FLAGS = -shared
	RELEASE_BUNDLE_FOLDER=CVector_Windows_$(ARCH)
else ifeq ($(SYSNAME),macOS)
	SHARED_LIB_FLAGS =  -install_name "@rpath/$(SHARED_LIB_VER_NAME)" \
		-dynamiclib -current_version \
		$(MAJOR_VERSION).$(MINOR_VERSION).$(PATCH_VERSION) \
		 -compatibility_version \
		 $(MAJOR_VERSION).$(MINOR_VERSION).$(PATCH_VERSION)
	RELEASE_BUNDLE_FOLDER=CVector_macOS_$(ARCH)
else
	SHARED_LIB_FLAGS = -shared -Wl,-soname,$(SHARED_LIB_VER_NAME)
	RELEASE_BUNDLE_FOLDER=CVector_Linux_$(ARCH)
endif

default: all

all: staticlib sharedlib

test-static: LIBS = -L $(LIBDIR)/static -lcvector$(LIBPOSTFIX)
test-static: staticlib $(TARGET)

test-shared: LDFLAGS = -Wl,-rpath '$(LIBDIR)/shared'
test-shared: LIBS = -L $(LIBDIR)/shared -lcvector$(LIBPOSTFIX)
test-shared: sharedlib $(TARGET)
test-shared:
ifeq ($(OS),Windows_NT)
	@cp $(SHARED_LIB_TARGET) ./$(SHARED_LIBNAME)
endif

staticlib: $(STATIC_LIB_TARGET)

sharedlib: CFLAGS += -fPIC
sharedlib: $(SHARED_LIB_TARGET)

INCLUDES = -I .

TESTOBJS = $(OBJDIR)/test.o
STATIC_LIBOBJS = $(STATIC_OBJDIR)/vector.o
SHARED_LIBOBJS = $(SHARED_OBJDIR)/vector.o

$(TARGET): $(TESTOBJS) 
	$(CC) $(TESTOBJS) -o $@ $(LIBS) $(LDFLAGS)

$(STATIC_LIB_TARGET): $(STATIC_LIBOBJS) 
	@mkdir -p $(@D)
	ar rcs $@ $^

$(SHARED_LIB_TARGET): $(SHARED_LIBOBJS) $(RES)
	@mkdir -p $(@D)
	$(CC) $(SHARED_LIB_FLAGS) $(RES) $(SHARED_LIBOBJS) -o $@
ifneq ($(SYSNAME),Windows)
	@cd $(LIBDIR)/shared/ && rm -rf $(SHARED_LIBNAME) && \
	ln -s $(SHARED_LIB_VER_NAME) $(SHARED_LIBNAME)
endif

$(OBJDIR)/%.res: %.rc
ifeq ($(SYSNAME),Windows)
	@mkdir -p $(@D)
	windres $(RCSDEBUG) $< -O coff -o $@
endif

$(OBJDIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(STATIC_OBJDIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(SHARED_OBJDIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

release-bundle:
	@$(MAKE) clean && $(MAKE) && $(MAKE) DEBUG=yes
	@mkdir -p $(RELEASE_BUNDLE_FOLDER)
	@cp vector.h LICENSE README.md $(RELEASE_BUNDLE_FOLDER)
	@cp -r $(LIBSDIR) $(RELEASE_BUNDLE_FOLDER)
ifeq ($(SYSNAME),Windows)
	tar -a -c -f $(RELEASE_BUNDLE_FOLDER).zip $(RELEASE_BUNDLE_FOLDER)
else ifeq ($(SYSNAME),macOS)
	hdiutil create -volname CVector \
	-srcfolder $(RELEASE_BUNDLE_FOLDER) -ov -format UDZO \
	$(RELEASE_BUNDLE_FOLDER).dmg
else
	tar -czf $(RELEASE_BUNDLE_FOLDER).tar.gz $(RELEASE_BUNDLE_FOLDER)/
endif

cleanwin:
ifeq ($(OS),Windows_NT)
	$(RM) -r *.dll
endif

cleanlibs: cleanwin
	$(RM) -r $(OBJSDIR)/release/log $(OBJSDIR)/debug/log $(LIBSDIR)

cleantest: cleanwin
	$(RM) -r $(OBJSDIR)/release/test $(OBJSDIR)/debug/test $(TARGET) $(LOGSDIR)

clean: cleanwin
	$(RM) -r $(OBJSDIR) $(TARGET) $(LIBSDIR) $(LOGSDIR) $(RELEASE_BUNDLE_FOLDER)*
