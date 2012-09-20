#
# OMNeT++/OMNEST Makefile for ClusterSys
#
# This file was generated with the command:
#  opp_makemake -f --deep -O out -I/Users/fernando/Documents/omnetpp-4.2.2/MiXiM/src/base/phyLayer -I/Users/fernando/Documents/omnetpp-4.2.2/MiXiM/src/base/modules -I/Users/fernando/Documents/omnetpp-4.2.2/MiXiM/src/modules/analogueModel -I/Users/fernando/Documents/omnetpp-4.2.2/MiXiM/src/modules/netw -I/Users/fernando/Documents/omnetpp-4.2.2/MiXiM/src/modules/messages -I/Users/fernando/Documents/omnetpp-4.2.2/MiXiM/src/base/connectionManager -I/Users/fernando/Documents/omnetpp-4.2.2/MiXiM/src/modules/utility -I/Users/fernando/Documents/omnetpp-4.2.2/MiXiM/src/base/messages -I/Users/fernando/Documents/omnetpp-4.2.2/MiXiM/src/base/utils -I/Users/fernando/Documents/omnetpp-4.2.2/MiXiM/src/modules/phy -L/Users/fernando/Documents/omnetpp-4.2.2/MiXiM/out/$(CONFIGNAME)/src/base -L/Users/fernando/Documents/omnetpp-4.2.2/MiXiM/out/$(CONFIGNAME)/src/modules -lmiximbase -lmiximmodules -KMIXIM_PROJ=/Users/fernando/Documents/omnetpp-4.2.2/MiXiM
#

# Name of target to be created (-o option)
TARGET = ClusterSys$(EXE_SUFFIX)

# User interface (uncomment one) (-u option)
USERIF_LIBS = $(ALL_ENV_LIBS) # that is, $(TKENV_LIBS) $(CMDENV_LIBS)
#USERIF_LIBS = $(CMDENV_LIBS)
#USERIF_LIBS = $(TKENV_LIBS)

# C++ include paths (with -I)
INCLUDE_PATH = \
    -I$(MIXIM_PROJ)/src/base/phyLayer \
    -I$(MIXIM_PROJ)/src/base/modules \
    -I$(MIXIM_PROJ)/src/modules/analogueModel \
    -I$(MIXIM_PROJ)/src/modules/netw \
    -I$(MIXIM_PROJ)/src/modules/messages \
    -I$(MIXIM_PROJ)/src/base/connectionManager \
    -I$(MIXIM_PROJ)/src/modules/utility \
    -I$(MIXIM_PROJ)/src/base/messages \
    -I$(MIXIM_PROJ)/src/base/utils \
    -I$(MIXIM_PROJ)/src/modules/phy \
    -I.

# Additional object and library files to link with
EXTRA_OBJS =

# Additional libraries (-L, -l options)
LIBS = -L$(MIXIM_PROJ)/out/$(CONFIGNAME)/src/base -L$(MIXIM_PROJ)/out/$(CONFIGNAME)/src/modules  -lmiximbase -lmiximmodules
LIBS += -Wl,-rpath,`abspath $(MIXIM_PROJ)/out/$(CONFIGNAME)/src/base` -Wl,-rpath,`abspath $(MIXIM_PROJ)/out/$(CONFIGNAME)/src/modules`

# Output directory
PROJECT_OUTPUT_DIR = out
PROJECTRELATIVE_PATH =
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cc and .msg files
OBJS = \
    $O/ClusterManager.o \
    $O/ClusterNetNetl.o \
    $O/Flood.o \
    $O/IClusterManger.o \
    $O/ClusterMCFA_m.o \
    $O/ClusterPkt_m.o \
    $O/NetwPkt_m.o

# Message files
MSGFILES = \
    ClusterMCFA.msg \
    ClusterPkt.msg \
    NetwPkt.msg

# Other makefile variables (-K)
MIXIM_PROJ=/Users/fernando/Documents/omnetpp-4.2.2/MiXiM

#------------------------------------------------------------------------------

# Pull in OMNeT++ configuration (Makefile.inc or configuser.vc)

ifneq ("$(OMNETPP_CONFIGFILE)","")
CONFIGFILE = $(OMNETPP_CONFIGFILE)
else
ifneq ("$(OMNETPP_ROOT)","")
CONFIGFILE = $(OMNETPP_ROOT)/Makefile.inc
else
CONFIGFILE = $(shell opp_configfilepath)
endif
endif

ifeq ("$(wildcard $(CONFIGFILE))","")
$(error Config file '$(CONFIGFILE)' does not exist -- add the OMNeT++ bin directory to the path so that opp_configfilepath can be found, or set the OMNETPP_CONFIGFILE variable to point to Makefile.inc)
endif

include $(CONFIGFILE)

# Simulation kernel and user interface libraries
OMNETPP_LIB_SUBDIR = $(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)
OMNETPP_LIBS = -L"$(OMNETPP_LIB_SUBDIR)" -L"$(OMNETPP_LIB_DIR)" -loppmain$D $(USERIF_LIBS) $(KERNEL_LIBS) $(SYS_LIBS)

COPTS = $(CFLAGS)  $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)
MSGCOPTS = $(INCLUDE_PATH)

# we want to recompile everything if COPTS changes,
# so we store COPTS into $COPTS_FILE and have object
# files depend on it (except when "make depend" was called)
COPTS_FILE = $O/.last-copts
ifneq ($(MAKECMDGOALS),depend)
ifneq ("$(COPTS)","$(shell cat $(COPTS_FILE) 2>/dev/null || echo '')")
$(shell $(MKPATH) "$O" && echo "$(COPTS)" >$(COPTS_FILE))
endif
endif

#------------------------------------------------------------------------------
# User-supplied makefile fragment(s)
# >>>
# <<<
#------------------------------------------------------------------------------

# Main target
all: $O/$(TARGET)
	$(LN) $O/$(TARGET) .

$O/$(TARGET): $(OBJS)  $(wildcard $(EXTRA_OBJS)) Makefile
	@$(MKPATH) $O
	$(CXX) $(LDFLAGS) -o $O/$(TARGET)  $(OBJS) $(EXTRA_OBJS) $(AS_NEEDED_OFF) $(WHOLE_ARCHIVE_ON) $(LIBS) $(WHOLE_ARCHIVE_OFF) $(OMNETPP_LIBS)

.PHONY: all clean cleanall depend msgheaders

.SUFFIXES: .cc

$O/%.o: %.cc $(COPTS_FILE)
	@$(MKPATH) $(dir $@)
	$(CXX) -c $(COPTS) -o $@ $<

%_m.cc %_m.h: %.msg
	$(MSGC) -s _m.cc $(MSGCOPTS) $?

msgheaders: $(MSGFILES:.msg=_m.h)

clean:
	-rm -rf $O
	-rm -f ClusterSys ClusterSys.exe libClusterSys.so libClusterSys.a libClusterSys.dll libClusterSys.dylib
	-rm -f ./*_m.cc ./*_m.h

cleanall: clean
	-rm -rf $(PROJECT_OUTPUT_DIR)

depend:
	$(MAKEDEPEND) $(INCLUDE_PATH) -f Makefile -P\$$O/ -- $(MSG_CC_FILES)  ./*.cc

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/ClusterMCFA_m.o: ClusterMCFA_m.cc \
	ClusterMCFA_m.h
$O/ClusterManager.o: ClusterManager.cc \
	ClusterManager.h \
	$(MIXIM_PROJ)/src/base/modules/BaseBattery.h \
	$(MIXIM_PROJ)/src/base/modules/BaseLayer.h \
	$(MIXIM_PROJ)/src/base/modules/BaseModule.h \
	$(MIXIM_PROJ)/src/base/modules/BaseNetwLayer.h \
	$(MIXIM_PROJ)/src/base/modules/BatteryAccess.h \
	$(MIXIM_PROJ)/src/base/utils/HostState.h \
	$(MIXIM_PROJ)/src/base/utils/MiXiMDefs.h \
	$(MIXIM_PROJ)/src/base/utils/PassedMessage.h \
	$(MIXIM_PROJ)/src/base/utils/SimpleAddress.h \
	$(MIXIM_PROJ)/src/base/utils/miximkerneldefs.h
$O/ClusterNetNetl.o: ClusterNetNetl.cc \
	ClusterNetNetl.h \
	ClusterPkt_m.h \
	NetwPkt_m.h \
	$(MIXIM_PROJ)/src/base/messages/NetwPkt_m.h \
	$(MIXIM_PROJ)/src/base/modules/AddressingInterface.h \
	$(MIXIM_PROJ)/src/base/modules/ArpInterface.h \
	$(MIXIM_PROJ)/src/base/modules/BaseBattery.h \
	$(MIXIM_PROJ)/src/base/modules/BaseLayer.h \
	$(MIXIM_PROJ)/src/base/modules/BaseMacLayer.h \
	$(MIXIM_PROJ)/src/base/modules/BaseModule.h \
	$(MIXIM_PROJ)/src/base/modules/BaseNetwLayer.h \
	$(MIXIM_PROJ)/src/base/modules/BatteryAccess.h \
	$(MIXIM_PROJ)/src/base/phyLayer/Interpolation.h \
	$(MIXIM_PROJ)/src/base/phyLayer/MappingBase.h \
	$(MIXIM_PROJ)/src/base/utils/FindModule.h \
	$(MIXIM_PROJ)/src/base/utils/HostState.h \
	$(MIXIM_PROJ)/src/base/utils/MiXiMDefs.h \
	$(MIXIM_PROJ)/src/base/utils/NetwControlInfo.h \
	$(MIXIM_PROJ)/src/base/utils/NetwToMacControlInfo.h \
	$(MIXIM_PROJ)/src/base/utils/PassedMessage.h \
	$(MIXIM_PROJ)/src/base/utils/SimpleAddress.h \
	$(MIXIM_PROJ)/src/base/utils/miximkerneldefs.h
$O/ClusterPkt_m.o: ClusterPkt_m.cc \
	ClusterPkt_m.h
$O/Flood.o: Flood.cc \
	Flood.h \
	NetwPkt_m.h \
	$(MIXIM_PROJ)/src/base/messages/NetwPkt_m.h \
	$(MIXIM_PROJ)/src/base/modules/BaseBattery.h \
	$(MIXIM_PROJ)/src/base/modules/BaseLayer.h \
	$(MIXIM_PROJ)/src/base/modules/BaseModule.h \
	$(MIXIM_PROJ)/src/base/modules/BaseNetwLayer.h \
	$(MIXIM_PROJ)/src/base/modules/BatteryAccess.h \
	$(MIXIM_PROJ)/src/base/utils/HostState.h \
	$(MIXIM_PROJ)/src/base/utils/MiXiMDefs.h \
	$(MIXIM_PROJ)/src/base/utils/NetwControlInfo.h \
	$(MIXIM_PROJ)/src/base/utils/PassedMessage.h \
	$(MIXIM_PROJ)/src/base/utils/SimpleAddress.h \
	$(MIXIM_PROJ)/src/base/utils/miximkerneldefs.h \
	$(MIXIM_PROJ)/src/modules/netw/Flood.h
$O/IClusterManger.o: IClusterManger.cc \
	IClusterManger.h
$O/NetwPkt_m.o: NetwPkt_m.cc \
	NetwPkt_m.h \
	$(MIXIM_PROJ)/src/base/messages/NetwPkt_m.h \
	$(MIXIM_PROJ)/src/base/utils/MiXiMDefs.h \
	$(MIXIM_PROJ)/src/base/utils/SimpleAddress.h \
	$(MIXIM_PROJ)/src/base/utils/miximkerneldefs.h

