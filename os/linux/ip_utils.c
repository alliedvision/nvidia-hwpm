/*
 * Copyright (c) 2021-2022, NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include <linux/slab.h>
#include <soc/tegra/fuse.h>
#include <uapi/linux/tegra-soc-hwpm-uapi.h>

#include <tegra_hwpm_kmem.h>
#include <tegra_hwpm_log.h>
#include <tegra_hwpm_io.h>
#include <tegra_hwpm.h>
#include <os/linux/ip_utils.h>
#include <os/linux/driver.h>

struct platform_device *tegra_soc_hwpm_pdev;
struct hwpm_ip_register_list *ip_register_list_head;

#define REGISTER_IP	true
#define UNREGISTER_IP	false

static int tegra_hwpm_alloc_ip_register_list_node(
	struct tegra_soc_hwpm_ip_ops *hwpm_ip_ops,
	struct hwpm_ip_register_list **node_ptr)
{
	struct hwpm_ip_register_list *new_node = NULL;

	new_node = kzalloc(sizeof(struct hwpm_ip_register_list), GFP_KERNEL);
	if (new_node == NULL) {
		tegra_hwpm_err(NULL,
			"struct hwpm_ip_register_list node allocation failed");
		return -ENOMEM;
	}
	new_node->next = NULL;

	/* Copy given ip register details to node */
	memcpy(&new_node->ip_ops, hwpm_ip_ops,
		sizeof(struct tegra_soc_hwpm_ip_ops));
	(*node_ptr) = new_node;

	return 0;
}

static int tegra_hwpm_note_ip_register(
	struct tegra_soc_hwpm_ip_ops *hwpm_ip_ops)
{
	int err = 0;
	struct hwpm_ip_register_list *node;

	if (ip_register_list_head == NULL) {
		err = tegra_hwpm_alloc_ip_register_list_node(hwpm_ip_ops,
			&ip_register_list_head);
		if (err != 0) {
			tegra_hwpm_err(NULL,
				"failed to note ip registration");
			return err;
		}
	} else {
		node = ip_register_list_head;
		while (node->next != NULL) {
			node = node->next;
		}

		err = tegra_hwpm_alloc_ip_register_list_node(hwpm_ip_ops,
			&node->next);
		if (err != 0) {
			tegra_hwpm_err(NULL,
				"failed to note ip registration");
			return err;
		}
	}

	return err;
}

void tegra_soc_hwpm_ip_register(struct tegra_soc_hwpm_ip_ops *hwpm_ip_ops)
{
	struct tegra_soc_hwpm *hwpm = NULL;
	struct tegra_hwpm_ip_ops ip_ops;
	int ret = 0;

	if (hwpm_ip_ops == NULL) {
		tegra_hwpm_err(NULL, "IP details missing");
		return;
	}

	if (tegra_soc_hwpm_pdev == NULL) {
		tegra_hwpm_dbg(hwpm, hwpm_info | hwpm_dbg_ip_register,
			"Noting IP 0x%llx register request",
			hwpm_ip_ops->ip_base_address);
		ret = tegra_hwpm_note_ip_register(hwpm_ip_ops);
		if (ret != 0) {
			tegra_hwpm_err(NULL,
				"Couldn't save IP register details");
			return;
		}
	} else {
		if (hwpm_ip_ops->ip_dev == NULL) {
			tegra_hwpm_err(hwpm, "IP dev to register is NULL");
			return;
		}
		hwpm = platform_get_drvdata(tegra_soc_hwpm_pdev);

		tegra_hwpm_dbg(hwpm, hwpm_info | hwpm_dbg_ip_register,
		"Register IP 0x%llx", hwpm_ip_ops->ip_base_address);

		ip_ops.ip_dev = hwpm_ip_ops->ip_dev;
		ip_ops.hwpm_ip_pm = hwpm_ip_ops->hwpm_ip_pm;
		ip_ops.hwpm_ip_reg_op = hwpm_ip_ops->hwpm_ip_reg_op;

		ret = hwpm->active_chip->extract_ip_ops(hwpm,
			hwpm_ip_ops->resource_enum,
			hwpm_ip_ops->ip_base_address, &ip_ops, REGISTER_IP);
		if (ret < 0) {
			tegra_hwpm_err(hwpm, "Failed to set IP ops for IP %d",
				hwpm_ip_ops->resource_enum);
		}
	}
}

void tegra_soc_hwpm_ip_unregister(struct tegra_soc_hwpm_ip_ops *hwpm_ip_ops)
{
	struct tegra_soc_hwpm *hwpm = NULL;
	struct tegra_hwpm_ip_ops ip_ops;
	int ret = 0;

	if (hwpm_ip_ops == NULL) {
		tegra_hwpm_err(NULL, "IP details missing");
		return;
	}

	if (tegra_soc_hwpm_pdev == NULL) {
		tegra_hwpm_dbg(hwpm, hwpm_info | hwpm_dbg_ip_register,
			"HWPM device not available");
	} else {
		if (hwpm_ip_ops->ip_dev == NULL) {
			tegra_hwpm_err(hwpm, "IP dev to unregister is NULL");
			return;
		}
		hwpm = platform_get_drvdata(tegra_soc_hwpm_pdev);

		tegra_hwpm_dbg(hwpm, hwpm_info | hwpm_dbg_ip_register,
		"Unregister IP 0x%llx", hwpm_ip_ops->ip_base_address);

		ip_ops.ip_dev = hwpm_ip_ops->ip_dev;
		ip_ops.hwpm_ip_pm = hwpm_ip_ops->hwpm_ip_pm;
		ip_ops.hwpm_ip_reg_op = hwpm_ip_ops->hwpm_ip_reg_op;

		ret = hwpm->active_chip->extract_ip_ops(hwpm,
			hwpm_ip_ops->resource_enum,
			hwpm_ip_ops->ip_base_address, &ip_ops, UNREGISTER_IP);
		if (ret < 0) {
			tegra_hwpm_err(hwpm, "Failed to reset IP ops for IP %d",
				hwpm_ip_ops->resource_enum);
		}
	}
}

int tegra_hwpm_complete_ip_register_impl(struct tegra_soc_hwpm *hwpm)
{
	int ret = 0;
	struct hwpm_ip_register_list *node = ip_register_list_head;
	struct tegra_hwpm_ip_ops ip_ops;

	tegra_hwpm_fn(hwpm, " ");

	while (node != NULL) {
		ip_ops.ip_dev = node->ip_ops.ip_dev;
		ip_ops.hwpm_ip_pm = node->ip_ops.hwpm_ip_pm;
		ip_ops.hwpm_ip_reg_op = node->ip_ops.hwpm_ip_reg_op;

		ret = hwpm->active_chip->extract_ip_ops(hwpm,
			node->ip_ops.resource_enum,
			node->ip_ops.ip_base_address, &ip_ops, true);
		if (ret != 0) {
			tegra_hwpm_err(hwpm,
				"Resource enum %d extract IP ops failed",
				node->ip_ops.resource_enum);
			return ret;
		}
		node = node->next;
	}
	return ret;
}

int tegra_hwpm_get_floorsweep_info(struct tegra_soc_hwpm *hwpm,
	struct tegra_soc_hwpm_ip_floorsweep_info *fs_info)
{
	int ret = 0;
	u32 i = 0U;

	tegra_hwpm_fn(hwpm, " ");

	for (i = 0U; i < fs_info->num_queries; i++) {
		ret = hwpm->active_chip->get_fs_info(
			hwpm, (u32)fs_info->ip_fsinfo[i].ip,
			&fs_info->ip_fsinfo[i].ip_inst_mask,
			&fs_info->ip_fsinfo[i].status);
		if (ret < 0) {
			/* Print error for debug purpose. */
			tegra_hwpm_err(hwpm, "Failed to get fs_info");
		}

		tegra_hwpm_dbg(hwpm, hwpm_info | hwpm_dbg_floorsweep_info,
			"Query %d: ip %d: ip_status: %d inst_mask 0x%llx",
			i, fs_info->ip_fsinfo[i].ip,
			fs_info->ip_fsinfo[i].status,
			fs_info->ip_fsinfo[i].ip_inst_mask);
	}
	return ret;
}

int tegra_hwpm_get_resource_info(struct tegra_soc_hwpm *hwpm,
	struct tegra_soc_hwpm_resource_info *rsrc_info)
{
	int ret = 0;
	u32 i = 0U;

	tegra_hwpm_fn(hwpm, " ");

	for (i = 0U; i < rsrc_info->num_queries; i++) {
		ret = hwpm->active_chip->get_resource_info(
			hwpm, (u32)rsrc_info->resource_info[i].resource,
			&rsrc_info->resource_info[i].status);
		if (ret < 0) {
			/* Print error for debug purpose. */
			tegra_hwpm_err(hwpm, "Failed to get rsrc_info");
		}

		tegra_hwpm_dbg(hwpm, hwpm_info | hwpm_dbg_resource_info,
			"Query %d: resource %d: status: %d",
			i, rsrc_info->resource_info[i].resource,
			rsrc_info->resource_info[i].status);
	}
	return ret;
}

void tegra_hwpm_release_ip_register_node(struct tegra_soc_hwpm *hwpm)
{
	struct hwpm_ip_register_list *node = ip_register_list_head;
	struct hwpm_ip_register_list *tmp_node = NULL;

	tegra_hwpm_fn(hwpm, " ");

	while (node != NULL) {
		tmp_node = node;
		node = tmp_node->next;
		tegra_hwpm_kfree(hwpm, tmp_node);
	}
}
