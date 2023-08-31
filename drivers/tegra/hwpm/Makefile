# Copyright (c) 2022-2023, NVIDIA CORPORATION. All rights reserved.
#
# Tegra SOC HWPM
#

GCOV_PROFILE := y

ifeq ($(origin srctree.hwpm), undefined)
# Set HWPM source repo path
srctree.hwpm := $(abspath $(shell dirname $(lastword $(MAKEFILE_LIST))))/../../..
endif

# For OOT builds, set required config flags
ifeq ($(CONFIG_TEGRA_OOT_MODULE),m)
NVHWPM_OBJ = m

CONFIG_TEGRA_HWPM_OOT := y
ccflags-y += -DCONFIG_TEGRA_HWPM_OOT

CONFIG_TEGRA_FUSE_UPSTREAM := y
ccflags-y += -DCONFIG_TEGRA_FUSE_UPSTREAM


LINUXINCLUDE += -I$(srctree.nvconftest)
LINUXINCLUDE += -I$(srctree.hwpm)/include
LINUXINCLUDE += -I$(srctree.hwpm)/drivers/tegra/hwpm/include
LINUXINCLUDE += -I$(srctree.hwpm)/drivers/tegra/hwpm

else
NVHWPM_OBJ = y

ccflags-y += -I$(srctree.hwpm)/include
ccflags-y += -I$(srctree.hwpm)/drivers/tegra/hwpm/include
ccflags-y += -I$(srctree.hwpm)/drivers/tegra/hwpm

endif

ifeq ($(or $(NV_BUILD_CONFIGURATION_IS_SAFETY),$(CONFIG_TEGRA_LINUX_PROD)),1)
nvhwpm-y := tegra_hwpm_mock.o

else
# Add required objects to nvhwpm object variable
include $(srctree.hwpm)/drivers/tegra/hwpm/Makefile.hwpm.sources

endif

obj-${NVHWPM_OBJ} += nvhwpm.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) $(ccflags) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

