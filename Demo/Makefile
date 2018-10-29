#############################################################
# Required variables for each makefile
# Discard this section from all parent makefiles
# Expected variables (with automatic defaults):
#   CSRCS (all "C" files in the dir)
#   SUBDIRS (all subdirs with a Makefile)
#   GEN_LIBS - list of libs to be generated ()
#   GEN_IMAGES - list of object file images to be generated ()
#   GEN_BINS - list of binaries to be generated ()
#   COMPONENTS_xxx - a list of libs/objs in the form
#     subdir/lib to be extracted and rolled up into
#     a generated lib/image xxx.a ()
#
TARGET = eagle
#FLAVOR = release
FLAVOR = debug

#EXTRA_CCFLAGS += -u

ifndef PDIR # {
GEN_IMAGES= eagle.app.v6.out
GEN_BINS= eagle.app.v6.bin
SPECIAL_MKTARGETS=$(APP_MKTARGETS)

####################################################################
# 子目录，如果你在该makfile文件目录里新建了一个源文件目录，需要添加到这里
# 否则不会编译未添加的目录的源文件！
SUBDIRS=    \
	user \
	userUart \
	userFlash 


endif # } PDIR

LDDIR = $(SDK_PATH)/ld

CCFLAGS += -Os

TARGET_LDFLAGS =		\
	-nostdlib		\
	-Wl,-EL \
	--longcalls \
	--text-section-literals

#############################################################
# 根据 FLAVOR 变量选择优化等级
# -O0不优化，-O2中级优化
ifeq ($(FLAVOR),debug)
    TARGET_LDFLAGS += -g -O2
endif

ifeq ($(FLAVOR),release)
    TARGET_LDFLAGS += -g -O0
endif


#############################################################
# 添加静态链接库，对应前面 SUBDIRS 变量设置的目录
# 每一个文件夹里的源代码先编译成对应的静态库，然后在进行链接
# 所以如果添加新的源码文件夹，也需要在这里添加对应的静态库！
COMPONENTS_eagle.app.v6 = \
	user/libuser.a \
	userUart/libuserUart.a \
	userFlash/libuserFlash.a 


#############################################################
# 链接参数
# 参考lib文件夹里的静态链接库
# 比如，如果需要使用smartconfig接口
# 只需要添加 -lsmartconfig 即可。
LINKFLAGS_eagle.app.v6 = \
	-L$(SDK_PATH)/lib        \
	-Wl,--gc-sections   \
	-nostdlib	\
    -T$(LD_FILE)   \
	-Wl,--no-check-sections	\
    -u call_user_start	\
	-Wl,-static						\
	-Wl,--start-group					\
	-lminic \
	-lgcc					\
	-lhal					\
	-lphy	\
	-lpp	\
	-lnet80211	\
	-lwpa	\
	-lcrypto    \
	-lmain	\
	-lfreertos	\
	-llwip	\
	-lespconn\
	-lsmartconfig \
	-lairkiss\
	$(DEP_LIBS_eagle.app.v6)					\
	-Wl,--end-group

DEPENDS_eagle.app.v6 = \
                $(LD_FILE) \
                $(LDDIR)/eagle.rom.addr.v6.ld

#############################################################
# Configuration i.e. compile options etc.
# Target specific stuff (defines etc.) goes in here!
# Generally values applying to a tree are captured in the
#   makefile at its root level - these are then overridden
#   for a subtree within the makefile rooted therein
#

#UNIVERSAL_TARGET_DEFINES =		\

# Other potential configuration flags include:
#	-DTXRX_TXBUF_DEBUG
#	-DTXRX_RXBUF_DEBUG
#	-DWLAN_CONFIG_CCX
CONFIGURATION_DEFINES =	-DICACHE_FLASH

DEFINES +=				\
	$(UNIVERSAL_TARGET_DEFINES)	\
	$(CONFIGURATION_DEFINES)

DDEFINES +=				\
	$(UNIVERSAL_TARGET_DEFINES)	\
	$(CONFIGURATION_DEFINES)


#############################################################
# Recursion Magic - Don't touch this!!
#
# Each subtree potentially has an include directory
#   corresponding to the common APIs applicable to modules
#   rooted at that subtree. Accordingly, the INCLUDE PATH
#   of a module can only contain the include directories up
#   its parent path, and not its siblings
#
# Required for each makefile to inherit from the parent
#

#############################################################
# 把当前目录下的 include 文件夹包含进来
INCLUDES := $(INCLUDES) -I \
            $(PDIR)include -I\
			$(PDIR)userUart \
			$(PDIR)userFlash
            
# 把子目录包含进来
sinclude $(SDK_PATH)/Makefile

.PHONY: FORCE
FORCE:

