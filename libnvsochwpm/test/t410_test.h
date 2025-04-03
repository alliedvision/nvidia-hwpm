/*
 * Copyright (c) 2025, NVIDIA CORPORATION.  All rights reserved.
 *
 * NVIDIA CORPORATION and its licensors retain all intellectual property
 * and proprietary rights in and to this software, related documentation
 * and any modifications thereto.  Any use, reproduction, disclosure or
 * distribution of this software and related documentation without an express
 * license agreement from NVIDIA CORPORATION is strictly prohibited.
 */

#ifndef T410_TEST_H
#define T410_TEST_H

#include "nv_soc_hwpm_test.h"

#define T410_MAX_SOCKETS 2

class T410Tests : public NvSocHwpmTests
{
public:
	T410Tests();
	~T410Tests() override;

protected:
	struct PmaConfigurationParams {
		PmaConfigurationParams()
		{
			enable_streaming = false;
			pulse_interval = 0;
			enable_pma_record = false;
			keep_latest = false;
		}

		bool enable_streaming;
		uint32_t pulse_interval;
		bool enable_pma_record;
		bool keep_latest;
	};

	struct PmmConfigurationParams {
		enum Mode {
			MODE_B,
			MODE_C,
			MODE_E
		};

		PmmConfigurationParams()
		{
			mode = MODE_B;
			perfmon_idx = 0;
			enable_local_triggering = false;
			enable_overflow_priming = false;
			collect_one = false;
		}


		Mode mode;
		uint32_t perfmon_idx;

		bool enable_local_triggering;
		bool enable_overflow_priming;
		bool collect_one;
	};

	void SetUp(void) override;
	void TearDown(void) override;

	void GetDevices(void);

	void TestRegopsRead(nv_soc_hwpm_session session,
		uint64_t pma_record_buffer_pma_va,
		size_t record_buffer_size);
	void TestRegopsWrite(nv_soc_hwpm_session session);

	void RegOpWrite32(
		nv_soc_hwpm_session session, uint64_t address, uint32_t value, uint32_t mask);
	void RegOpRead32(
		nv_soc_hwpm_session session, uint64_t address, uint32_t *value);

	void SetupPma(nv_soc_hwpm_session session, const PmaConfigurationParams &params);
	void EnablePmaStreaming(nv_soc_hwpm_session session, const PmaConfigurationParams &params);
	void SetupPmm(nv_soc_hwpm_session session, const PmmConfigurationParams &params);
	void SetupWatchbus(nv_soc_hwpm_session session, const PmmConfigurationParams &params);
	void TeardownPma(nv_soc_hwpm_session session);
	void TeardownPmm(nv_soc_hwpm_session session, const PmmConfigurationParams &params);
	void TeardownPerfmux(nv_soc_hwpm_session session);
	void IssuePmaTrigger(nv_soc_hwpm_session session);
	void HarvestCounters(
		nv_soc_hwpm_session session,
		const PmmConfigurationParams &params,
		const uint32_t sig_val[4]);

	nv_soc_hwpm_device t410_dev[T410_MAX_SOCKETS];
	uint32_t t410_dev_count;
};

#endif // T410_TEST_H
