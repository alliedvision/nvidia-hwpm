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
#ifndef TH500_HWPM_IP_C2C_H
#define TH500_HWPM_IP_C2C_H

#if defined(CONFIG_TH500_HWPM_IP_C2C)
#define TH500_HWPM_ACTIVE_IP_C2C               TH500_HWPM_IP_C2C,

/* This data should ideally be available in HW headers */
#define TH500_HWPM_IP_C2C_NUM_INSTANCES                2U
#define TH500_HWPM_IP_C2C_NUM_CORE_ELEMENT_PER_INST    1U
#define TH500_HWPM_IP_C2C_NUM_PERFMON_PER_INST         1U
#define TH500_HWPM_IP_C2C_NUM_PERFMUX_PER_INST         5U
#define TH500_HWPM_IP_C2C_NUM_BROADCAST_PER_INST       1U

extern struct hwpm_ip th500_hwpm_ip_c2c;

#else
#define TH500_HWPM_ACTIVE_IP_C2C
#endif

#endif /* TH500_HWPM_IP_C2C_H */

