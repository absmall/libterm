ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)
EXTRA_INCVPATH= $(PROJECT_ROOT)/../include

NAME=term

include $(MKFILES_ROOT)/qtargets.mk
