version := $(subst -, ,$(shell git describe --long --dirty --tags))
COMMIT := $(strip $(word 3, $(version)))
COMMITS_PAST := $(strip $(word 2, $(version)))
DIRTY := $(strip $(word 4, $(version)))
ifneq ($(COMMITS_PAST), 0)
	BUILD_INFO_COMMITS := "."$(COMMITS_PAST)
endif
ifneq ($(DIRTY),)
	BUILD_INFO_DIRTY :="+"
endif

export BUILD_TAG :=$(strip $(word 1, $(version)))
export BUILD_INFO := $(COMMIT)$(BUILD_INFO_COMMITS)$(BUILD_INFO_DIRTY)

print_version:
	@echo "BUILD_TAG: $(BUILD_TAG)"
	@echo "BUILD_INFO: $(BUILD_INFO)"

CFLAGS += -DVERSION_BUILD_DATE=\""$(shell date)"\" \
          -DVERSION_BUILD_MACHINE=\""$(shell echo `whoami`@`hostname`)"\" \
          -DVERSION_TAG=\"$(BUILD_TAG\" \
          -DVERSION_BUILD=\"$(BUILD_INFO)\"
