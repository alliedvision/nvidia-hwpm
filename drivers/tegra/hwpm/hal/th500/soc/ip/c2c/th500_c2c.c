/*
 * Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

#include "th500_c2c.h"

#include <tegra_hwpm.h>
#include <hal/th500/soc/th500_soc_perfmon_device_index.h>
#include <hal/th500/soc/th500_soc_regops_allowlist.h>
#include <hal/th500/soc/hw/th500_addr_map_soc_hwpm.h>

static struct hwpm_ip_aperture th500_c2c_inst0_perfmon_element_static_array[
	TH500_HWPM_IP_C2C_NUM_PERFMON_PER_INST] = {
	{
		.element_type = HWPM_ELEMENT_PERFMON,
		.element_index_mask = BIT(0),
		.element_index = 0U,
		.dt_mmio = NULL,
		.name = "perfmon_c2c0",
		.device_index = TH500_CTC0_PERFMON_DEVICE_NODE_INDEX,
		.start_abs_pa = addr_map_rpg_pm_ctc0_base_r(),
		.end_abs_pa = addr_map_rpg_pm_ctc0_limit_r(),
		.start_pa = addr_map_rpg_pm_ctc0_base_r(),
		.end_pa = addr_map_rpg_pm_ctc0_limit_r(),
		.base_pa = addr_map_rpg_pm_base_r(),
		.alist = th500_perfmon_alist,
		.alist_size = ARRAY_SIZE(th500_perfmon_alist),
		.fake_registers = NULL,
	},
};

static struct hwpm_ip_aperture th500_c2c_inst1_perfmon_element_static_array[
	TH500_HWPM_IP_C2C_NUM_PERFMON_PER_INST] = {
	{
		.element_type = HWPM_ELEMENT_PERFMON,
		.element_index_mask = BIT(0),
		.element_index = 0U,
		.dt_mmio = NULL,
		.name = "perfmon_c2c1",
		.device_index = TH500_CTC1_PERFMON_DEVICE_NODE_INDEX,
		.start_abs_pa = addr_map_rpg_pm_ctc1_base_r(),
		.end_abs_pa = addr_map_rpg_pm_ctc1_limit_r(),
		.start_pa = addr_map_rpg_pm_ctc1_base_r(),
		.end_pa = addr_map_rpg_pm_ctc1_limit_r(),
		.base_pa = addr_map_rpg_pm_base_r(),
		.alist = th500_perfmon_alist,
		.alist_size = ARRAY_SIZE(th500_perfmon_alist),
		.fake_registers = NULL,
	},
};

static struct hwpm_ip_aperture th500_c2c_inst0_perfmux_element_static_array[
	TH500_HWPM_IP_C2C_NUM_PERFMUX_PER_INST] = {
	{
		.element_type = IP_ELEMENT_PERFMUX,
		.element_index_mask = BIT(0),
		.element_index = 0U,
		.dt_mmio = NULL,
		.name = {'\0'},
		.start_abs_pa = addr_map_c2c0_base_r(),
		.end_abs_pa = addr_map_c2c0_limit_r(),
		.start_pa = addr_map_c2c0_base_r(),
		.end_pa = addr_map_c2c0_limit_r(),
		.base_pa = 0ULL,
		.alist = th500_c2c_alist,
		.alist_size = ARRAY_SIZE(th500_c2c_alist),
		.fake_registers = NULL,
	},
	{
		.element_type = IP_ELEMENT_PERFMUX,
		.element_index_mask = BIT(1),
		.element_index = 1U,
		.dt_mmio = NULL,
		.name = {'\0'},
		.start_abs_pa = addr_map_c2c1_base_r(),
		.end_abs_pa = addr_map_c2c1_limit_r(),
		.start_pa = addr_map_c2c1_base_r(),
		.end_pa = addr_map_c2c1_limit_r(),
		.base_pa = 0ULL,
		.alist = th500_c2c_alist,
		.alist_size = ARRAY_SIZE(th500_c2c_alist),
		.fake_registers = NULL,
	},
	{
		.element_type = IP_ELEMENT_PERFMUX,
		.element_index_mask = BIT(2),
		.element_index = 2U,
		.dt_mmio = NULL,
		.name = {'\0'},
		.start_abs_pa = addr_map_c2c2_base_r(),
		.end_abs_pa = addr_map_c2c2_limit_r(),
		.start_pa = addr_map_c2c2_base_r(),
		.end_pa = addr_map_c2c2_limit_r(),
		.base_pa = 0ULL,
		.alist = th500_c2c_alist,
		.alist_size = ARRAY_SIZE(th500_c2c_alist),
		.fake_registers = NULL,
	},
	{
		.element_type = IP_ELEMENT_PERFMUX,
		.element_index_mask = BIT(3),
		.element_index = 3U,
		.dt_mmio = NULL,
		.name = {'\0'},
		.start_abs_pa = addr_map_c2c3_base_r(),
		.end_abs_pa = addr_map_c2c3_limit_r(),
		.start_pa = addr_map_c2c3_base_r(),
		.end_pa = addr_map_c2c3_limit_r(),
		.base_pa = 0ULL,
		.alist = th500_c2c_alist,
		.alist_size = ARRAY_SIZE(th500_c2c_alist),
		.fake_registers = NULL,
	},
	{
		.element_type = IP_ELEMENT_PERFMUX,
		.element_index_mask = BIT(4),
		.element_index = 4U,
		.dt_mmio = NULL,
		.name = {'\0'},
		.start_abs_pa = addr_map_c2c4_base_r(),
		.end_abs_pa = addr_map_c2c4_limit_r(),
		.start_pa = addr_map_c2c4_base_r(),
		.end_pa = addr_map_c2c4_limit_r(),
		.base_pa = 0ULL,
		.alist = th500_c2c_alist,
		.alist_size = ARRAY_SIZE(th500_c2c_alist),
		.fake_registers = NULL,
	},
};

static struct hwpm_ip_aperture th500_c2c_inst1_perfmux_element_static_array[
	TH500_HWPM_IP_C2C_NUM_PERFMUX_PER_INST] = {
	{
		.element_type = IP_ELEMENT_PERFMUX,
		.element_index_mask = BIT(0),
		.element_index = 0U,
		.dt_mmio = NULL,
		.name = {'\0'},
		.start_abs_pa = addr_map_c2c5_base_r(),
		.end_abs_pa = addr_map_c2c5_limit_r(),
		.start_pa = addr_map_c2c5_base_r(),
		.end_pa = addr_map_c2c5_limit_r(),
		.base_pa = 0ULL,
		.alist = th500_c2c_alist,
		.alist_size = ARRAY_SIZE(th500_c2c_alist),
		.fake_registers = NULL,
	},
	{
		.element_type = IP_ELEMENT_PERFMUX,
		.element_index_mask = BIT(1),
		.element_index = 1U,
		.dt_mmio = NULL,
		.name = {'\0'},
		.start_abs_pa = addr_map_c2c6_base_r(),
		.end_abs_pa = addr_map_c2c6_limit_r(),
		.start_pa = addr_map_c2c6_base_r(),
		.end_pa = addr_map_c2c6_limit_r(),
		.base_pa = 0ULL,
		.alist = th500_c2c_alist,
		.alist_size = ARRAY_SIZE(th500_c2c_alist),
		.fake_registers = NULL,
	},
	{
		.element_type = IP_ELEMENT_PERFMUX,
		.element_index_mask = BIT(2),
		.element_index = 2U,
		.dt_mmio = NULL,
		.name = {'\0'},
		.start_abs_pa = addr_map_c2c7_base_r(),
		.end_abs_pa = addr_map_c2c7_limit_r(),
		.start_pa = addr_map_c2c7_base_r(),
		.end_pa = addr_map_c2c7_limit_r(),
		.base_pa = 0ULL,
		.alist = th500_c2c_alist,
		.alist_size = ARRAY_SIZE(th500_c2c_alist),
		.fake_registers = NULL,
	},
	{
		.element_type = IP_ELEMENT_PERFMUX,
		.element_index_mask = BIT(3),
		.element_index = 3U,
		.dt_mmio = NULL,
		.name = {'\0'},
		.start_abs_pa = addr_map_c2c8_base_r(),
		.end_abs_pa = addr_map_c2c8_limit_r(),
		.start_pa = addr_map_c2c8_base_r(),
		.end_pa = addr_map_c2c8_limit_r(),
		.base_pa = 0ULL,
		.alist = th500_c2c_alist,
		.alist_size = ARRAY_SIZE(th500_c2c_alist),
		.fake_registers = NULL,
	},
	{
		.element_type = IP_ELEMENT_PERFMUX,
		.element_index_mask = BIT(4),
		.element_index = 4U,
		.dt_mmio = NULL,
		.name = {'\0'},
		.start_abs_pa = addr_map_c2c9_base_r(),
		.end_abs_pa = addr_map_c2c9_limit_r(),
		.start_pa = addr_map_c2c9_base_r(),
		.end_pa = addr_map_c2c9_limit_r(),
		.base_pa = 0ULL,
		.alist = th500_c2c_alist,
		.alist_size = ARRAY_SIZE(th500_c2c_alist),
		.fake_registers = NULL,
	},

};

static struct hwpm_ip_aperture th500_c2c_inst0_broadcast_element_static_array[
	TH500_HWPM_IP_C2C_NUM_BROADCAST_PER_INST] = {
	{
		.element_type = IP_ELEMENT_BROADCAST,
		.element_index_mask = BIT(0),
		.element_index = 0U,
		.dt_mmio = NULL,
		.name = {'\0'},
		.start_abs_pa = addr_map_c2cs0_base_r(),
		.end_abs_pa = addr_map_c2cs0_limit_r(),
		.start_pa = 0ULL,
		.end_pa = 0ULL,
		.base_pa = 0ULL,
		.alist = th500_c2c_alist,
		.alist_size = ARRAY_SIZE(th500_c2c_alist),
		.fake_registers = NULL,
	},
};

struct hwpm_ip_aperture th500_c2c_inst1_broadcast_element_static_array[
	TH500_HWPM_IP_C2C_NUM_BROADCAST_PER_INST] = {
	{
		.element_type = IP_ELEMENT_BROADCAST,
		.element_index_mask = BIT(1),
		.element_index = 1U,
		.dt_mmio = NULL,
		.name = {'\0'},
		.start_abs_pa = addr_map_c2cs1_base_r(),
		.end_abs_pa = addr_map_c2cs1_limit_r(),
		.start_pa = 0ULL,
		.end_pa = 0ULL,
		.base_pa = 0ULL,
		.alist = th500_c2c_alist,
		.alist_size = ARRAY_SIZE(th500_c2c_alist),
		.fake_registers = NULL,
	},
};

/* IP instance array */
static struct hwpm_ip_inst th500_c2c_inst_static_array[
	TH500_HWPM_IP_C2C_NUM_INSTANCES] = {
	{
		.hw_inst_mask = BIT(0),
		.num_core_elements_per_inst =
			TH500_HWPM_IP_C2C_NUM_CORE_ELEMENT_PER_INST,
		.element_info = {
			/*
			 * Instance info corresponding to
			 * TEGRA_HWPM_APERTURE_TYPE_PERFMUX
			 */
			{
				.num_element_per_inst =
					TH500_HWPM_IP_C2C_NUM_PERFMUX_PER_INST,
				.element_static_array =
					th500_c2c_inst0_perfmux_element_static_array,
				.range_start = addr_map_c2c0_base_r(),
				.range_end = addr_map_c2c4_limit_r(),
				.element_stride = addr_map_c2c0_limit_r() -
					addr_map_c2c0_base_r() + 1ULL,
				.element_slots = 0U,
				.element_arr = NULL,
			},
			/*
			 * Instance info corresponding to
			 * TEGRA_HWPM_APERTURE_TYPE_BROADCAST
			 */
			{
				.num_element_per_inst =
					TH500_HWPM_IP_C2C_NUM_BROADCAST_PER_INST,
				.element_static_array =
					th500_c2c_inst0_broadcast_element_static_array,
				.range_start = addr_map_c2cs0_base_r(),
				.range_end = addr_map_c2cs0_limit_r(),
				.element_stride = addr_map_c2cs0_limit_r() -
					addr_map_c2cs0_base_r() + 1ULL,
				.element_slots = 0U,
				.element_arr = NULL,
			},
			/*
			 * Instance info corresponding to
			 * TEGRA_HWPM_APERTURE_TYPE_PERFMON
			 */
			{
				.num_element_per_inst =
					TH500_HWPM_IP_C2C_NUM_PERFMON_PER_INST,
				.element_static_array =
					th500_c2c_inst0_perfmon_element_static_array,
				.range_start = addr_map_rpg_pm_ctc0_base_r(),
				.range_end = addr_map_rpg_pm_ctc0_limit_r(),
				.element_stride = addr_map_rpg_pm_ctc0_limit_r() -
					addr_map_rpg_pm_ctc0_base_r() + 1ULL,
				.element_slots = 0U,
				.element_arr = NULL,
			},
		},
		.ip_ops = {
			.ip_dev = NULL,
			.hwpm_ip_pm = NULL,
			.hwpm_ip_reg_op = NULL,
		},

		.element_fs_mask = 0U,
	},
	{
		.hw_inst_mask = BIT(1),
		.num_core_elements_per_inst =
			TH500_HWPM_IP_C2C_NUM_CORE_ELEMENT_PER_INST,
		.element_info = {
			/*
			 * Instance info corresponding to
			 * TEGRA_HWPM_APERTURE_TYPE_PERFMUX
			 */
			{
				.num_element_per_inst =
					TH500_HWPM_IP_C2C_NUM_PERFMUX_PER_INST,
				.element_static_array =
					th500_c2c_inst1_perfmux_element_static_array,
				.range_start = addr_map_c2c5_base_r(),
				.range_end = addr_map_c2c9_limit_r(),
				.element_stride = addr_map_c2c5_limit_r() -
					addr_map_c2c5_base_r() + 1ULL,
				.element_slots = 0U,
				.element_arr = NULL,
			},
			/*
			 * Instance info corresponding to
			 * TEGRA_HWPM_APERTURE_TYPE_BROADCAST
			 */
			{
				.num_element_per_inst =
					TH500_HWPM_IP_C2C_NUM_BROADCAST_PER_INST,
				.element_static_array =
					th500_c2c_inst1_broadcast_element_static_array,
				.range_start = addr_map_c2cs1_base_r(),
				.range_end = addr_map_c2cs1_limit_r(),
				.element_stride = addr_map_c2cs1_limit_r() -
					addr_map_c2cs1_base_r() + 1ULL,
				.element_slots = 0U,
				.element_arr = NULL,
			},
			/*
			 * Instance info corresponding to
			 * TEGRA_HWPM_APERTURE_TYPE_PERFMON
			 */
			{
				.num_element_per_inst =
					TH500_HWPM_IP_C2C_NUM_PERFMON_PER_INST,
				.element_static_array =
					th500_c2c_inst1_perfmon_element_static_array,
				.range_start = addr_map_rpg_pm_ctc1_base_r(),
				.range_end = addr_map_rpg_pm_ctc1_limit_r(),
				.element_stride = addr_map_rpg_pm_ctc1_limit_r() -
					addr_map_rpg_pm_ctc1_base_r() + 1ULL,
				.element_slots = 0U,
				.element_arr = NULL,
			},
		},
		.ip_ops = {
			.ip_dev = NULL,
			.hwpm_ip_pm = NULL,
			.hwpm_ip_reg_op = NULL,
		},

		.element_fs_mask = 0U,
	},
};

/* IP structure */
struct hwpm_ip th500_hwpm_ip_c2c = {
	.num_instances = TH500_HWPM_IP_C2C_NUM_INSTANCES,
	.ip_inst_static_array = th500_c2c_inst_static_array,

	.inst_aperture_info = {
		/*
		 * Instance info corresponding to
		 * TEGRA_HWPM_APERTURE_TYPE_PERFMUX
		 */
		{
			.range_start = addr_map_c2c0_base_r(),
			.range_end = addr_map_c2c9_limit_r(),
			.inst_stride = addr_map_c2c4_limit_r() -
				addr_map_c2c0_base_r() + 1ULL,
			.inst_slots = 0U,
			.inst_arr = NULL,
		},
		/*
		 * Instance info corresponding to
		 * TEGRA_HWPM_APERTURE_TYPE_BROADCAST
		 */
		{
			.range_start = addr_map_c2cs0_base_r(),
			.range_end = addr_map_c2cs1_limit_r(),
			.inst_stride = addr_map_c2cs0_limit_r() -
				addr_map_c2cs0_base_r() + 1ULL,
			.inst_slots = 0U,
			.inst_arr = NULL,
		},
		/*
		 * Instance info corresponding to
		 * TEGRA_HWPM_APERTURE_TYPE_PERFMON
		 */
		{
			.range_start = addr_map_rpg_pm_ctc0_base_r(),
			.range_end = addr_map_rpg_pm_ctc1_limit_r(),
			.inst_stride = addr_map_rpg_pm_ctc0_limit_r() -
				addr_map_rpg_pm_ctc0_base_r() + 1ULL,
			.inst_slots = 0U,
			.inst_arr = NULL,
		},
	},
	.dependent_fuse_mask = TEGRA_HWPM_FUSE_SECURITY_MODE_MASK |
		TEGRA_HWPM_FUSE_HWPM_GLOBAL_DISABLE_MASK,
	.override_enable = false,
	.inst_fs_mask = 0U,
	.resource_status = TEGRA_HWPM_RESOURCE_STATUS_INVALID,
	.reserved = false,
};
