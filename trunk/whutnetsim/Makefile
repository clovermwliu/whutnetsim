AR        := $(shell which ar)
RANLIB    := $(shell which ranlib)
TOPDIR	  ?= $(shell echo `(cd .. && pwd)`)


include $(TOPDIR)/mk/oscheck.mk
include $(TOPDIR)/mk/qtcheck.mk
include $(TOPDIR)/mk/rticheck.mk
export

SOURCES   := $(wildcard *.cc)
BASENAMES := $(basename $(SOURCES)) 
DBGOBJDIR := $(TOPDIR)/OBJDBG
DBGOBJMODELSDIR := $(TOPDIR)/OBJDBG/Models
DDPNDPRFX := $(addsuffix \/, $(subst /,\/, $(DBGOBJDIR)))
OPTOBJDIR := $(TOPDIR)/OBJOPT
OPTOBJMODELSDIR := $(TOPDIR)/OBJOPT/Models
ODPNDPRFX := $(addsuffix \/, $(subst /,\/, $(OPTOBJDIR)))
OBJECTS   := $(subst .cc,.o,$(SOURCES))

INCLUDES  := $(QT_INCL) $(RTI_INCL) -I$(TOPDIR)/SRC
LIBS      := $(QT_LIBS) $(RTI_LIBS)

DOBJECTS := $(addprefix $(DBGOBJDIR)/, $(addsuffix .o, $(basename $(SOURCES))))

OOBJECTS := $(addprefix $(OPTOBJDIR)/, $(addsuffix .o, $(basename $(SOURCES))))

DYN_LIB_DBG := $(TOPDIR)/libGTNetS-debug.$(SHARED_EXT)
DYN_LIB_OPT := $(TOPDIR)/libGTNetS-opt.$(SHARED_EXT)

STA_LIB_DBG := $(TOPDIR)/libGTNetS-debug.$(STATIC_EXT)
STA_LIB_OPT := $(TOPDIR)/libGTNetS-opt.$(STATIC_EXT)


$(DBGOBJDIR):
	$(MKDIR) -p $(DBGOBJDIR)
	$(MKDIR) -p $(DBGOBJMODELSDIR)

$(OPTOBJDIR):
	$(MKDIR) -p $(OPTOBJDIR)
	$(MKDIR) -p $(OPTOBJMODELSDIR)

all:    debug opt

moc_qtwindow.h: qtwindow.h
ifdef HAVE_QT
	@echo Running MOC
	$(MOC) $< -o $@ ;
else
	@echo No libqt found
endif

debug:	$(STA_LIB_DBG) $(DYN_LIB_DBG) moc_qtwindow.h

opt:	$(STA_LIB_OPT) $(DYN_LIB_OPT) moc_qtwindow.h

$(STA_LIB_DBG): $(DOBJECTS)
	@echo Making GTNetS static library with debugging symbols
	$(AR) -r $@ $(DOBJECTS)
	$(RANLIB) $@

$(DYN_LIB_DBG): $(DOBJECTS)
	@echo Making GTNetS shared library with debugging symbols
ifeq (Darwin,$(OSNAME))
	$(CXX) $(SHFLAGS) $@ -o $@ $(DOBJECTS) $(RTI_LIBS) $(QT_LIBS)
else
	$(CXX) $(SHFLAGS) $(PICFLAGS) -o $@ $(DOBJECTS) $(RTI_LIBS)
endif

$(STA_LIB_OPT): $(OOBJECTS)
	@echo Making optimized GTNetS static library
	$(AR) -r $@ $(OOBJECTS)
	$(RANLIB) $@


$(DYN_LIB_OPT): $(OOBJECTS)
	@echo Making optimized  GTNetS shared library
ifeq (Darwin,$(OSNAME))
	$(CXX) $(SHFLAGS) $@ -o $@ $(OOBJECTS) $(RTI_LIBS) $(QT_LIBS)
else
	$(CXX) $(SHFLAGS) $(PICFLAGS) -o $@ $(OOBJECTS) $(RTI_LIBS)
endif


$(DBGOBJDIR)/%.o::%.cc moc_qtwindow.h
	@echo Compiling debug $*
	$(CXX) -g $(PICFLAGS) $(CFLAGS) $(INCLUDES) -o $@ -c $*.cc

$(OPTOBJDIR)/%.o::%.cc moc_qtwindow.h
	@echo Compiling opt $*
	$(CXX) $(COPTS) $(PICFLAGS) $(CFLAGS) $(INCLUDES) -o $@ -c $*.cc

clean:
	@echo Cleaning in SRC
	find $(DBGOBJDIR) $(OPTOBJDIR) -name '*.o' -exec rm {} \;

cleanall: clean
	@echo Cleaning all in SRC
	rm -rf $(DBGOBJDIR) $(OPTOBJDIR) make.depend.* moc_qtwindow.h

.PHONY: clean cleanall

depend: $(SOURCES) $(DBGOBJDIR) $(OPTOBJDIR)
	@echo Computing Dependencies in SRC
	$(CXX) $(CFLAGS) $(INCLUDES) $(DFLAGS) $(SOURCES) 2>/dev/null | sed  -e 's/.*\.o/$(DDPNDPRFX)&/' > make.depend.dbg
	$(CXX) $(CFLAGS) $(INCLUDES) $(DFLAGS) $(SOURCES) 2>/dev/null | sed  -e 's/.*\.o/$(ODPNDPRFX)&/' > make.depend.opt

ifeq (make.depend.dbg, $(findstring make.depend.dbg, $(wildcard *)))
include make.depend.dbg
endif
ifeq (make.depend.opt, $(findstring make.depend.opt, $(wildcard *)))
include make.depend.opt
endif


# this target is for debugging only
testing:
	@echo $(SOURCES)
	@echo $(DOBJECTS) 
	@echo $(OOBJECTS) 
	@echo $(CFLAGS) 
	@echo $(INCLUDES)
	@echo $(LIBS)
