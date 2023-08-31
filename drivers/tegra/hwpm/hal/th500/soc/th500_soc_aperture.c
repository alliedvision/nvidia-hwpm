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

#include <tegra_hwpm_timers.h>
#include <tegra_hwpm_log.h>
#include <tegra_hwpm_io.h>
#include <tegra_hwpm.h>
#include <hal/th500/th500_internal.h>
#include <hal/th500/soc/th500_soc_internal.h>

#include <hal/th500/soc/hw/th500_pmasys_soc_hwpm.h>
#include <hal/th500/soc/hw/th500_pmmsys_soc_hwpm.h>

int th500_hwpm_soc_check_status(struct tegra_soc_hwpm *hwpm)
{
	int err = 0;
	u32 reg_val = 0U;
	struct tegra_soc_hwpm_chip *active_chip = hwpm->active_chip;
	struct hwpm_ip *chip_ip = active_chip->chip_ips[
		active_chip->get_rtr_int_idx(hwpm)];
	struct hwpm_ip_inst *ip_inst_rtr = &chip_ip->ip_inst_static_array[
		TH500_HWPM_IP_RTR_STATIC_RTR_INST];
	struct hwpm_ip_inst *ip_inst_pma = &chip_ip->ip_inst_static_array[
		TH500_HWPM_IP_RTR_STATIC_PMA_INST];
	struct hwpm_ip_aperture *rtr_perfmux = &ip_inst_rtr->element_info[
		TEGRA_HWPM_APERTURE_TYPE_PERFMUX].element_static_array[
			TH500_HWPM_IP_RTR_PERFMUX_INDEX];
	struct hwpm_ip_aperture *pma_perfmux = &ip_inst_pma->element_info[
		TEGRA_HWPM_APERTURE_TYPE_PERFMUX].element_static_array[
			TH500_HWPM_IP_RTR_PERFMUX_INDEX];

	tegra_hwpm_fn(hwpm, " ");

	/* Check ROUTER state */
	err = tegra_hwpm_readl(hwpm, rtr_perfmux,
		pmmsys_sys0router_enginestatus_r(), &reg_val);
	if (err != 0) {
		tegra_hwpm_err(hwpm, "hwpm read failed");
		return err;
	}

	if (pmmsys_sys0router_enginestatus_status_v(reg_val) !=
			pmmsys_sys0router_enginestatus_status_empty_v()) {
		tegra_hwpm_err(hwpm, "Router not ready value 0x%x", reg_val);
		return -EINVAL;
	}

	/* Check PMA state */
	err = tegra_hwpm_readl(hwpm, pma_perfmux,
		pmasys_channel_status_r(0), &reg_val);
	if (err != 0) {
		tegra_hwpm_err(hwpm, "hwpm read failed");
		return err;
	}

	if ((reg_val & pmasys_channel_status_engine_status_m()) !=
			pmasys_channel_status_engine_status_empty_f()) {
		tegra_hwpm_err(hwpm, "PMA not ready value 0x%x", reg_val);
		return -EINVAL;
	}

	return err;
}

int th500_hwpm_soc_disable_triggers(struct tegra_soc_hwpm *hwpm)
{
	int err = 0;
	u32 reg_val = 0U;
	u32 sleep_msecs = 100;
	struct tegra_hwpm_timeout timeout;
	struct tegra_soc_hwpm_chip *active_chip = hwpm->active_chip;
	struct hwpm_ip *chip_ip = active_chip->chip_ips[
		active_chip->get_rtr_int_idx(hwpm)];
	struct hwpm_ip_inst *ip_inst_rtr = &chip_ip->ip_inst_static_array[
		TH500_HWPM_IP_RTR_STATIC_RTR_INST];
	struct hwpm_ip_inst *ip_inst_pma = &chip_ip->ip_inst_static_array[
		TH500_HWPM_IP_RTR_STATIC_PMA_INST];
	struct hwpm_ip_aperture *rtr_perfmux = &ip_inst_rtr->element_info[
		TEGRA_HWPM_APERTURE_TYPE_PERFMUX].element_static_array[
			TH500_HWPM_IP_RTR_PERFMUX_INDEX];
	struct hwpm_ip_aperture *pma_perfmux = &ip_inst_pma->element_info[
		TEGRA_HWPM_APERTURE_TYPE_PERFMUX].element_static_array[
			TH500_HWPM_IP_RTR_PERFMUX_INDEX];

	tegra_hwpm_fn(hwpm, " ");

	/* Disable PMA triggers */
	err = tegra_hwpm_readl(hwpm, pma_perfmux,
		pmasys_command_slice_trigger_config_user_r(0), &reg_val);
	if (err != 0) {
		tegra_hwpm_err(hwpm, "hwpm read failed");
		return err;
	}
	reg_val = set_field(reg_val,
		pmasys_command_slice_trigger_config_user_pma_pulse_m(),
		pmasys_command_slice_trigger_config_user_pma_pulse_disable_f());
	err = tegra_hwpm_writel(hwpm, pma_perfmux,
		pmasys_command_slice_trigger_config_user_r(0), reg_val);
	if (err != 0) {
		tegra_hwpm_err(hwpm, "hwpm write failed");
		return err;
	}

	/* Wait for PERFMONs to idle */
	err = tegra_hwpm_timeout_init(hwpm, &timeout, 10U);
	if (err != 0) {
		tegra_hwpm_err(hwpm, "hwpm timeout init failed");
		return err;
	}

	do {
		err = tegra_hwpm_readl(hwpm, rtr_perfmux,
			pmmsys_sysrouter_enginestatus_r(), &reg_val);
		if (err != 0) {
			tegra_hwpm_err(hwpm, "hwpm read failed");
			return err;
		}
		tegra_hwpm_msleep(sleep_msecs);
	} while ((pmmsys_sysrouter_enginestatus_merged_perfmon_status_v(
			reg_val) != 0U) &&
		(tegra_hwpm_timeout_expired(hwpm, &timeout) == 0));

	if (pmmsys_sysrouter_enginestatus_merged_perfmon_status_v(
			reg_val) != 0U) {
		tegra_hwpm_err(hwpm, "Timeout expired for "
			"NV_PERF_PMMSYS_SYSROUTER_ENGINESTATUS_PERFMON_STATUS");
		return -ETIMEDOUT;
	}

	/* Wait for ROUTER to idle */
	err = tegra_hwpm_timeout_init(hwpm, &timeout, 10U);
	if (err != 0) {
		tegra_hwpm_err(hwpm, "hwpm timeout init failed");
		return err;
	}

	do {
		err = tegra_hwpm_readl(hwpm, rtr_perfmux,
			pmmsys_sys0router_enginestatus_r(), &reg_val);
		if (err != 0) {
			tegra_hwpm_err(hwpm, "hwpm read failed");
			return err;
		}
		tegra_hwpm_msleep(sleep_msecs);
	} while ((pmmsys_sys0router_enginestatus_status_v(reg_val) !=
			pmmsys_sys0router_enginestatus_status_empty_v()) &&
		(tegra_hwpm_timeout_expired(hwpm, &timeout) == 0));

	if (pmmsys_sys0router_enginestatus_status_v(reg_val) !=
			pmmsys_sys0router_enginestatus_status_empty_v()) {
		tegra_hwpm_err(hwpm, "Timeout expired for "
			"NV_PERF_PMMSYS_SYS0ROUTER_ENGINESTATUS_STATUS");
		return -ETIMEDOUT;
	}

	/* Wait for PMA to idle */
	err = tegra_hwpm_timeout_init(hwpm, &timeout, 10U);
	if (err != 0) {
		tegra_hwpm_err(hwpm, "hwpm timeout init failed");
		return err;
	}

	do {
		err = tegra_hwpm_readl(hwpm, pma_perfmux,
			pmasys_channel_status_r(0), &reg_val);
		if (err != 0) {
			tegra_hwpm_err(hwpm, "hwpm read failed");
			return err;
		}
		tegra_hwpm_msleep(sleep_msecs);
	} while (((reg_val & pmasys_channel_status_engine_status_m()) !=
			pmasys_channel_status_engine_status_empty_f()) &&
		(tegra_hwpm_timeout_expired(hwpm, &timeout) == 0));

	if ((reg_val & pmasys_channel_status_engine_status_m()) !=
			pmasys_channel_status_engine_status_empty_f()) {
		tegra_hwpm_err(hwpm, "Timeout expired for "
			"NV_PERF_PMASYS_CHANNEL_STATUS");
		return -ETIMEDOUT;
	}

	return err;
}

int th500_hwpm_soc_init_prod_values(struct tegra_soc_hwpm *hwpm)
{
	int err = 0;
	u32 reg_val = 0U;
	struct tegra_soc_hwpm_chip *active_chip = hwpm->active_chip;
	struct hwpm_ip *chip_ip = active_chip->chip_ips[
		active_chip->get_rtr_int_idx(hwpm)];
	struct hwpm_ip_inst *ip_inst_rtr = &chip_ip->ip_inst_static_array[
		TH500_HWPM_IP_RTR_STATIC_RTR_INST];
	struct hwpm_ip_inst *ip_inst_pma = &chip_ip->ip_inst_static_array[
		TH500_HWPM_IP_RTR_STATIC_PMA_INST];
	struct hwpm_ip_aperture *rtr_perfmux = &ip_inst_rtr->element_info[
		TEGRA_HWPM_APERTURE_TYPE_PERFMUX].element_static_array[
			TH500_HWPM_IP_RTR_PERFMUX_INDEX];
	struct hwpm_ip_aperture *pma_perfmux = &ip_inst_pma->element_info[
		TEGRA_HWPM_APERTURE_TYPE_PERFMUX].element_static_array[
			TH500_HWPM_IP_RTR_PERFMUX_INDEX];

	tegra_hwpm_fn(hwpm, " ");

	err = tegra_hwpm_readl(hwpm, pma_perfmux,
		pmasys_channel_config_user_r(0), &reg_val);
	if (err != 0) {
		tegra_hwpm_err(hwpm, "hwpm read failed");
		return err;
	}
	reg_val = set_field(reg_val,
		pmasys_channel_config_user_coalesce_timeout_cycles_m(),
		pmasys_channel_config_user_coalesce_timeout_cycles__prod_f());
	err = tegra_hwpm_writel(hwpm, pma_perfmux,
		pmasys_channel_config_user_r(0), reg_val);
	if (err != 0) {
		tegra_hwpm_err(hwpm, "hwpm write failed");
		return err;
	}

	err = tegra_hwpm_readl(hwpm, pma_perfmux, pmasys_cg2_r(), &reg_val);
	if (err != 0) {
		tegra_hwpm_err(hwpm, "hwpm read failed");
		return err;
	}
	reg_val = set_field(reg_val, pmasys_cg2_slcg_m(),
			pmasys_cg2_slcg__prod_f());
	err = tegra_hwpm_writel(hwpm, pma_perfmux, pmasys_cg2_r(), reg_val);
	if (err != 0) {
		tegra_hwpm_err(hwpm, "hwpm write failed");
		return err;
	}

	err = tegra_hwpm_readl(hwpm, rtr_perfmux,
		pmmsys_sys0router_cg1_secure_r(), &reg_val);
	if (err != 0) {
		tegra_hwpm_err(hwpm, "hwpm read failed");
		return err;
	}
	reg_val = set_field(reg_val,
		pmmsys_sys0router_cg1_secure_flcg_perfmon_m(),
		pmmsys_sys0router_cg1_secure_flcg_perfmon__prod_f());
	err = tegra_hwpm_writel(hwpm, rtr_perfmux,
		pmmsys_sys0router_cg1_secure_r(), reg_val);
	if (err != 0) {
		tegra_hwpm_err(hwpm, "hwpm write failed");
		return err;
	}

	err = tegra_hwpm_readl(hwpm, rtr_perfmux,
		pmmsys_sys0router_perfmon_cg2_secure_r(), &reg_val);
	if (err != 0) {
		tegra_hwpm_err(hwpm, "hwpm read failed");
		return err;
	}
	reg_val = set_field(reg_val,
		pmmsys_sys0router_perfmon_cg2_secure_slcg_m(),
		pmmsys_sys0router_perfmon_cg2_secure_slcg__prod_f());
	err = tegra_hwpm_writel(hwpm, rtr_perfmux,
		pmmsys_sys0router_perfmon_cg2_secure_r(), reg_val);
	if (err != 0) {
		tegra_hwpm_err(hwpm, "hwpm write failed");
		return err;
	}

	return 0;
}

int th500_hwpm_soc_disable_cg(struct tegra_soc_hwpm *hwpm)
{
	int err = 0;
	u32 reg_val = 0U;
	struct tegra_soc_hwpm_chip *active_chip = hwpm->active_chip;
	struct hwpm_ip *chip_ip = active_chip->chip_ips[
		active_chip->get_rtr_int_idx(hwpm)];
	struct hwpm_ip_inst *ip_inst_rtr = &chip_ip->ip_inst_static_array[
		TH500_HWPM_IP_RTR_STATIC_RTR_INST];
	struct hwpm_ip_inst *ip_inst_pma = &chip_ip->ip_inst_static_array[
		TH500_HWPM_IP_RTR_STATIC_PMA_INST];
	struct hwpm_ip_aperture *rtr_perfmux = &ip_inst_rtr->element_info[
		TEGRA_HWPM_APERTURE_TYPE_PERFMUX].element_static_array[
			TH500_HWPM_IP_RTR_PERFMUX_INDEX];
	struct hwpm_ip_aperture *pma_perfmux = &ip_inst_pma->element_info[
		TEGRA_HWPM_APERTURE_TYPE_PERFMUX].element_static_array[
			TH500_HWPM_IP_RTR_PERFMUX_INDEX];

	err = tegra_hwpm_readl(hwpm, pma_perfmux, pmasys_cg2_r(), &reg_val);
	if (err != 0) {
		tegra_hwpm_err(hwpm, "hwpm read failed");
		return err;
	}
	reg_val = set_field(reg_val, pmasys_cg2_slcg_m(),
			pmasys_cg2_slcg_disabled_f());
	err = tegra_hwpm_writel(hwpm, pma_perfmux, pmasys_cg2_r(), reg_val);
	if (err != 0) {
		tegra_hwpm_err(hwpm, "hwpm write failed");
		return err;
	}

	err = tegra_hwpm_readl(hwpm, rtr_perfmux,
		pmmsys_sys0router_cg1_secure_r(), &reg_val);
	if (err != 0) {
		tegra_hwpm_err(hwpm, "hwpm read failed");
		return err;
	}
	reg_val = set_field(reg_val,
		pmmsys_sys0router_cg1_secure_flcg_perfmon_m(),
		pmmsys_sys0router_cg1_secure_flcg_perfmon_disabled_f());
	err = tegra_hwpm_writel(hwpm, rtr_perfmux,
		pmmsys_sys0router_cg1_secure_r(), reg_val);
	if (err != 0) {
		tegra_hwpm_err(hwpm, "hwpm write failed");
		return err;
	}

	err = tegra_hwpm_readl(hwpm, rtr_perfmux,
		pmmsys_sys0router_perfmon_cg2_secure_r(), &reg_val);
	if (err != 0) {
		tegra_hwpm_err(hwpm, "hwpm read failed");
		return err;
	}
	reg_val = set_field(reg_val,
		pmmsys_sys0router_perfmon_cg2_secure_slcg_m(),
		pmmsys_sys0router_perfmon_cg2_secure_slcg_disabled_f());
	err = tegra_hwpm_writel(hwpm, rtr_perfmux,
		pmmsys_sys0router_perfmon_cg2_secure_r(), reg_val);
	if (err != 0) {
		tegra_hwpm_err(hwpm, "hwpm write failed");
		return err;
	}

	err = tegra_hwpm_readl(hwpm, rtr_perfmux,
		pmmsys_sys0router_cg2_r(), &reg_val);
	if (err != 0) {
		tegra_hwpm_err(hwpm, "hwpm read failed");
		return err;
	}
	reg_val = set_field(reg_val,
		pmmsys_sys0router_cg2_slcg_m(),
		pmmsys_sys0router_cg2_slcg_disabled_f());
	err = tegra_hwpm_writel(hwpm, rtr_perfmux,
		pmmsys_sys0router_cg2_r(), reg_val);
	if (err != 0) {
		tegra_hwpm_err(hwpm, "hwpm write failed");
		return err;
	}

	return 0;
}

int th500_hwpm_soc_enable_cg(struct tegra_soc_hwpm *hwpm)
{
	int err = 0;
	u32 reg_val = 0U;
	struct tegra_soc_hwpm_chip *active_chip = hwpm->active_chip;
	struct hwpm_ip *chip_ip = active_chip->chip_ips[
		active_chip->get_rtr_int_idx(hwpm)];
	struct hwpm_ip_inst *ip_inst_rtr = &chip_ip->ip_inst_static_array[
		TH500_HWPM_IP_RTR_STATIC_RTR_INST];
	struct hwpm_ip_inst *ip_inst_pma = &chip_ip->ip_inst_static_array[
		TH500_HWPM_IP_RTR_STATIC_PMA_INST];
	struct hwpm_ip_aperture *rtr_perfmux = &ip_inst_rtr->element_info[
		TEGRA_HWPM_APERTURE_TYPE_PERFMUX].element_static_array[
			TH500_HWPM_IP_RTR_PERFMUX_INDEX];
	struct hwpm_ip_aperture *pma_perfmux = &ip_inst_pma->element_info[
		TEGRA_HWPM_APERTURE_TYPE_PERFMUX].element_static_array[
			TH500_HWPM_IP_RTR_PERFMUX_INDEX];

	err = tegra_hwpm_readl(hwpm, pma_perfmux, pmasys_cg2_r(), &reg_val);
	if (err != 0) {
		tegra_hwpm_err(hwpm, "hwpm read failed");
		return err;
	}
	reg_val = set_field(reg_val, pmasys_cg2_slcg_m(),
			pmasys_cg2_slcg_enabled_f());
	err = tegra_hwpm_writel(hwpm, pma_perfmux, pmasys_cg2_r(), reg_val);
	if (err != 0) {
		tegra_hwpm_err(hwpm, "hwpm write failed");
		return err;
	}

	err = tegra_hwpm_readl(hwpm, rtr_perfmux,
		pmmsys_sys0router_cg1_secure_r(), &reg_val);
	if (err != 0) {
		tegra_hwpm_err(hwpm, "hwpm read failed");
		return err;
	}
	reg_val = set_field(reg_val,
		pmmsys_sys0router_cg1_secure_flcg_perfmon_m(),
		pmmsys_sys0router_cg1_secure_flcg_perfmon_enabled_f());
	err = tegra_hwpm_writel(hwpm, rtr_perfmux,
		pmmsys_sys0router_cg1_secure_r(), reg_val);
	if (err != 0) {
		tegra_hwpm_err(hwpm, "hwpm write failed");
		return err;
	}

	err = tegra_hwpm_readl(hwpm, rtr_perfmux,
		pmmsys_sys0router_perfmon_cg2_secure_r(), &reg_val);
	if (err != 0) {
		tegra_hwpm_err(hwpm, "hwpm read failed");
		return err;
	}
	reg_val = set_field(reg_val,
		pmmsys_sys0router_perfmon_cg2_secure_slcg_m(),
		pmmsys_sys0router_perfmon_cg2_secure_slcg_enabled_f());
	err = tegra_hwpm_writel(hwpm, rtr_perfmux,
		pmmsys_sys0router_perfmon_cg2_secure_r(), reg_val);
	if (err != 0) {
		tegra_hwpm_err(hwpm, "hwpm write failed");
		return err;
	}

	err = tegra_hwpm_readl(hwpm, rtr_perfmux,
		pmmsys_sys0router_cg2_r(), &reg_val);
	if (err != 0) {
		tegra_hwpm_err(hwpm, "hwpm read failed");
		return err;
	}
	reg_val = set_field(reg_val,
		pmmsys_sys0router_cg2_slcg_m(),
		pmmsys_sys0router_cg2_slcg_enabled_f());
	err = tegra_hwpm_writel(hwpm, rtr_perfmux,
		pmmsys_sys0router_cg2_r(), reg_val);
	if (err != 0) {
		tegra_hwpm_err(hwpm, "hwpm write failed");
		return err;
	}

	return 0;
}
