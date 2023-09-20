// SPDX-License-Identifier: MIT
/* SPDX-FileCopyrightText: Copyright (c) 2022-2023, NVIDIA CORPORATION & AFFILIATES.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <tegra_hwpm.h>
#include <tegra_hwpm_io.h>
#include <tegra_hwpm_log.h>
#include <tegra_hwpm_soc.h>
#include <tegra_hwpm_common.h>
#include <tegra_hwpm_static_analysis.h>

#include <hal/th500/th500_internal.h>
#include <hal/th500/soc/th500_soc_internal.h>
#include <hal/th500/soc/hw/th500_addr_map_soc_hwpm.h>

/*
 * This function is invoked by register_ip API.
 * Convert the external resource enum to internal IP index.
 * Extract given ip_ops and update corresponding IP structure.
 */
int th500_hwpm_extract_ip_ops(struct tegra_soc_hwpm *hwpm,
	u32 resource_enum, u64 base_address,
	struct tegra_hwpm_ip_ops *ip_ops, bool available)
{
	int ret = 0;
	u32 ip_idx = 0U;

	tegra_hwpm_fn(hwpm, " ");

	tegra_hwpm_dbg(hwpm, hwpm_dbg_ip_register,
		"Extract IP ops for resource enum %d info", resource_enum);

	/* Convert tegra_soc_hwpm_resource to internal enum */
	if (!(hwpm->active_chip->is_resource_active(hwpm,
		resource_enum, &ip_idx))) {
		tegra_hwpm_dbg(hwpm, hwpm_dbg_ip_register,
			"SOC hwpm resource %d (base 0x%llx) is unconfigured",
			resource_enum, base_address);
		goto fail;
	}

	switch (ip_idx) {
#if defined(CONFIG_TH500_HWPM_IP_CL2)
	case TH500_HWPM_IP_CL2:
#endif
#if defined(CONFIG_TH500_HWPM_IP_SMMU)
	case TH500_HWPM_IP_SMMU:
#endif
#if defined(CONFIG_TH500_HWPM_IP_C_NVLINK)
	case TH500_HWPM_IP_C_NVLINK:
#endif
#if defined(CONFIG_TH500_HWPM_IP_PCIE)
	case TH500_HWPM_IP_PCIE:
#endif
#if defined(CONFIG_TH500_HWPM_IP_C2C)
	case TH500_HWPM_IP_C2C:
#endif
#if defined(CONFIG_TH500_HWPM_IP_CL2) ||		\
	defined(CONFIG_TH500_HWPM_IP_SMMU) ||		\
	defined(CONFIG_TH500_HWPM_IP_C_NVLINK) ||	\
	defined(CONFIG_TH500_HWPM_IP_PCIE) ||		\
	defined(CONFIG_TH500_HWPM_IP_C2C)
		ret = tegra_hwpm_set_fs_info_ip_ops(hwpm, ip_ops,
			base_address, ip_idx, available);
		if (ret != 0) {
			tegra_hwpm_err(hwpm,
				"IP %d base 0x%llx:Failed to %s fs/ops",
				ip_idx, base_address,
				available == true ? "set" : "reset");
			goto fail;
		}
		break;
#endif
#if defined(CONFIG_TH500_HWPM_IP_MSS_CHANNEL)
	case TH500_HWPM_IP_MSS_CHANNEL:
#endif
#if defined(CONFIG_TH500_HWPM_IP_MCF_CORE)
	case TH500_HWPM_IP_MCF_CORE:
#endif
#if defined(CONFIG_TH500_HWPM_IP_MCF_CLINK)
	case TH500_HWPM_IP_MCF_CLINK:
#endif
#if defined(CONFIG_TH500_HWPM_IP_MCF_C2C)
    case TH500_HWPM_IP_MCF_C2C:
#endif
#if defined(CONFIG_TH500_HWPM_IP_MCF_SOC)
    case TH500_HWPM_IP_MCF_SOC:
#endif
#if defined(CONFIG_TH500_HWPM_IP_SOC_HUB)
    case TH500_HWPM_IP_SOC_HUB:
#endif
		/*
		 * MSS channel, MCF CORE, MCF CLINK, MCF C2C, MCF SOC,
		 * and SOC HUB share MC channels
		 */

		/* Check base address in TH500_HWPM_IP_MSS_CHANNEL */
#if defined(CONFIG_TH500_HWPM_IP_MSS_CHANNEL)
		ip_idx = TH500_HWPM_IP_MSS_CHANNEL;
		ret = tegra_hwpm_set_fs_info_ip_ops(hwpm, ip_ops,
			base_address, ip_idx, available);
		if (ret != 0) {
			/*
			 * Return value of ENODEV will indicate that the base
			 * address doesn't belong to this IP.
			 * This case is valid, as not all base addresses are
			 * shared between MSS IPs.
			 * In this case, reset return value to 0.
			 */
			if (ret != -ENODEV) {
				tegra_hwpm_err(hwpm,
					"IP %d base 0x%llx:Failed to %s fs/ops",
					ip_idx, base_address,
					available == true ? "set" : "reset");
				goto fail;
			}
			ret = 0;
		}
#endif
#if defined(CONFIG_TH500_HWPM_IP_MCF_CORE)
		/* Check base address in TH500_HWPM_IP_MCF_CORE */
		ip_idx = TH500_HWPM_IP_MCF_CORE;
		ret = tegra_hwpm_set_fs_info_ip_ops(hwpm, ip_ops,
			base_address, ip_idx, available);
		if (ret != 0) {
			/*
			 * Return value of ENODEV will indicate that the base
			 * address doesn't belong to this IP.
			 * This case is valid, as not all base addresses are
			 * shared between MSS IPs.
			 * In this case, reset return value to 0.
			 */
			if (ret != -ENODEV) {
				tegra_hwpm_err(hwpm,
					"IP %d base 0x%llx:Failed to %s fs/ops",
					ip_idx, base_address,
					available == true ? "set" : "reset");
				goto fail;
			}
			ret = 0;
		}
#endif
#if defined(CONFIG_TH500_HWPM_IP_MCF_CLINK)
		/* Check base address in TH500_HWPM_IP_MCF_CLINK */
		ip_idx = TH500_HWPM_IP_MCF_CLINK;
		ret = tegra_hwpm_set_fs_info_ip_ops(hwpm, ip_ops,
			base_address, ip_idx, available);
		if (ret != 0) {
			/*
			 * Return value of ENODEV will indicate that the base
			 * address doesn't belong to this IP.
			 * This case is valid, as not all base addresses are
			 * shared between MSS IPs.
			 * In this case, reset return value to 0.
			 */
			if (ret != -ENODEV) {
				tegra_hwpm_err(hwpm,
					"IP %d base 0x%llx:Failed to %s fs/ops",
					ip_idx, base_address,
					available == true ? "set" : "reset");
				goto fail;
			}
			ret = 0;
		}
#endif
#if defined(CONFIG_TH500_HWPM_IP_MCF_C2C)
		/* Check base address in TH500_HWPM_IP_MCF_C2C */
		ip_idx = TH500_HWPM_IP_MCF_C2C;
		ret = tegra_hwpm_set_fs_info_ip_ops(hwpm, ip_ops,
			base_address, ip_idx, available);
		if (ret != 0) {
			/*
			 * Return value of ENODEV will indicate that the base
			 * address doesn't belong to this IP.
			 * This case is valid, as not all base addresses are
			 * shared between MSS IPs.
			 * In this case, reset return value to 0.
			 */
			if (ret != -ENODEV) {
				tegra_hwpm_err(hwpm,
					"IP %d base 0x%llx:Failed to %s fs/ops",
					ip_idx, base_address,
					available == true ? "set" : "reset");
				goto fail;
			}
			ret = 0;
		}
#endif
#if defined(CONFIG_TH500_HWPM_IP_MCF_SOC)
		/* Check base address in TH500_HWPM_IP_MCF_SOC */
		ip_idx = TH500_HWPM_IP_MCF_SOC;
		ret = tegra_hwpm_set_fs_info_ip_ops(hwpm, ip_ops,
			base_address, ip_idx, available);
		if (ret != 0) {
			/*
			 * Return value of ENODEV will indicate that the base
			 * address doesn't belong to this IP.
			 * This case is valid, as not all base addresses are
			 * shared between MSS IPs.
			 * In this case, reset return value to 0.
			 */
			if (ret != -ENODEV) {
				tegra_hwpm_err(hwpm,
					"IP %d base 0x%llx:Failed to %s fs/ops",
					ip_idx, base_address,
					available == true ? "set" : "reset");
				goto fail;
			}
			ret = 0;
		}
#endif
#if defined(CONFIG_TH500_HWPM_IP_SOC_HUB)
		/* Check base address in TH500_HWPM_IP_SOC_HUB */
		ip_idx = TH500_HWPM_IP_SOC_HUB;
		ret = tegra_hwpm_set_fs_info_ip_ops(hwpm, ip_ops,
			base_address, ip_idx, available);
		if (ret != 0) {
			/*
			 * Return value of ENODEV will indicate that the base
			 * address doesn't belong to this IP.
			 * This case is valid, as not all base addresses are
			 * shared between MSS IPs.
			 * In this case, reset return value to 0.
			 */
			if (ret != -ENODEV) {
				tegra_hwpm_err(hwpm,
					"IP %d base 0x%llx:Failed to %s fs/ops",
					ip_idx, base_address,
					available == true ? "set" : "reset");
				goto fail;
			}
			ret = 0;
		}
#endif
		break;
	case TH500_HWPM_IP_PMA:
	case TH500_HWPM_IP_RTR:
	default:
		tegra_hwpm_err(hwpm, "Invalid IP %d for ip_ops", ip_idx);
		break;
	}

fail:
	return ret;
}

int th500_hwpm_validate_current_config(struct tegra_soc_hwpm *hwpm)
{
	u32 production_mode = 0U;
	u32 security_mode = 0U;
	u32 fa_mode = 0U;
	u32 hwpm_global_disable = 0U;
	u32 opt_hwpm_disable = 0U;
	u32 idx = 0U;
	int err;
	struct tegra_soc_hwpm_chip *active_chip = hwpm->active_chip;
	struct hwpm_ip *chip_ip = NULL;

	tegra_hwpm_fn(hwpm, " ");

	if (!tegra_hwpm_is_platform_silicon()) {
		return 0;
	}

	/* Read production mode fuse */
	err = tegra_hwpm_fuse_readl_prod_mode(hwpm, &production_mode);
	if (err != 0) {
		tegra_hwpm_err(hwpm, "prod mode fuse read failed");
		return err;
	}

#define TEGRA_FUSE_SECURITY_MODE		0xA0U
	err = tegra_hwpm_fuse_readl(
		hwpm, TEGRA_FUSE_SECURITY_MODE, &security_mode);
	if (err != 0) {
		tegra_hwpm_err(hwpm, "security mode fuse read failed");
		return err;
	}

#define TEGRA_FUSE_OPT_HWPM_DISABLE		0x2cU
	err = tegra_hwpm_fuse_readl(
		hwpm, TEGRA_FUSE_SECURITY_MODE, &opt_hwpm_disable);
	if (err != 0) {
		tegra_hwpm_err(hwpm, "opt hwpm disable fuse read failed");
		return err;
	}

#define TEGRA_FUSE_FA_MODE			0x48U
	err = tegra_hwpm_fuse_readl(hwpm, TEGRA_FUSE_FA_MODE, &fa_mode);
	if (err != 0) {
		tegra_hwpm_err(hwpm, "fa mode fuse read failed");
		return err;
	}

#define TEGRA_HWPM_GLOBAL_DISABLE_OFFSET	0x3CU
#define TEGRA_HWPM_GLOBAL_DISABLE_DISABLED	0x1U
	err = tegra_hwpm_read_sticky_bits(hwpm, addr_map_pmc_misc_base_r(),
		TEGRA_HWPM_GLOBAL_DISABLE_OFFSET, &hwpm_global_disable);
	if (err != 0) {
		tegra_hwpm_err(hwpm, "hwpm global disable read failed");
		return err;
	}

	tegra_hwpm_dbg(hwpm, hwpm_info,
		"PROD_MODE fuse = 0x%x "
		"SECURITY_MODE fuse = 0x%x "
		"HWPM disable fuse = 0x%x"
		"FA mode fuse = 0x%x"
		"HWPM_GLOBAL_DISABLE sticky bit = 0x%x",
		production_mode, security_mode, opt_hwpm_disable,
		fa_mode, hwpm_global_disable);

	/* Do not enable override if FA mode fuse is set */
	if (fa_mode != 0U) {
		tegra_hwpm_dbg(hwpm, hwpm_info,
			"fa mode fuse enabled, no override required");
		return 0;
	}

	/* Override enable depends on security mode and global hwpm disable */
	if ((security_mode == 0U) &&
		(hwpm_global_disable == TEGRA_HWPM_GLOBAL_DISABLE_DISABLED)) {
		tegra_hwpm_dbg(hwpm, hwpm_info,
			"security fuses are disabled, no override required");
		return 0;
	}

	for (idx = 0U; idx < active_chip->get_ip_max_idx(); idx++) {
		chip_ip = active_chip->chip_ips[idx];

		if ((hwpm_global_disable !=
			TEGRA_HWPM_GLOBAL_DISABLE_DISABLED) &&
			((chip_ip->dependent_fuse_mask &
			TEGRA_HWPM_FUSE_HWPM_GLOBAL_DISABLE_MASK) != 0U)) {
			/* HWPM disable is true */
			/* IP depends on HWPM global disable */
			chip_ip->override_enable = true;
		} else {
			/* HWPM disable is false */
			if ((security_mode != 0U) &&
				((chip_ip->dependent_fuse_mask &
				TEGRA_HWPM_FUSE_SECURITY_MODE_MASK) != 0U)) {
				/* Security mode fuse is set */
				/* IP depends on security mode fuse */
				chip_ip->override_enable = true;
			} else {
				/*
				 * This is a valid case since not all IPs
				 * depend on security fuse.
				 */
				tegra_hwpm_dbg(hwpm, hwpm_info,
					"IP %d not overridden", idx);
			}
		}
	}

	return 0;
}

int th500_hwpm_force_enable_ips(struct tegra_soc_hwpm *hwpm)
{
	int ret = 0, err = 0;

	tegra_hwpm_fn(hwpm, " ");

#if defined(CONFIG_TH500_HWPM_ALLOW_FORCE_ENABLE)
	/* MSS CHANNEL */
#if defined(CONFIG_TH500_HWPM_IP_MSS_CHANNEL)
	ret = tegra_hwpm_set_fs_info_ip_ops(hwpm, NULL,
		addr_map_mc0_base_r(), TH500_HWPM_IP_MSS_CHANNEL, true);
	if (ret != 0) {
		tegra_hwpm_err(hwpm,
			"TH500_HWPM_IP_MSS_CHANNEL force enable failed");
		err = ret;
	}
#endif
#if defined(CONFIG_TH500_HWPM_IP_C2C)
	/* CTC Link */
	ret = tegra_hwpm_set_fs_info_ip_ops(hwpm, NULL,
		addr_map_c2c0_base_r(), TH500_HWPM_IP_C2C, true);
	if (ret != 0) {
		tegra_hwpm_err(hwpm,
			"TH500_HWPM_IP_C2C force enable failed");
		return ret;
	}

	ret = tegra_hwpm_set_fs_info_ip_ops(hwpm, NULL,
		addr_map_c2c5_base_r(), TH500_HWPM_IP_C2C, true);
	if (ret != 0) {
		tegra_hwpm_err(hwpm,
			"TH500_HWPM_IP_C2C force enable failed");
		return ret;
	}
#endif
#endif
	return err;
}
