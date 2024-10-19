/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ifndef __SBI_ECALL_INTERFACE_H__
#define __SBI_ECALL_INTERFACE_H__

/* SBI Extension IDs */
#define SBI_EXT_0_1_SET_TIMER			0x0
#define SBI_EXT_0_1_CONSOLE_PUTCHAR		0x1
#define SBI_EXT_0_1_CONSOLE_GETCHAR		0x2
#define SBI_EXT_0_1_CLEAR_IPI			0x3
#define SBI_EXT_0_1_SEND_IPI			0x4
#define SBI_EXT_0_1_REMOTE_FENCE_I		0x5
#define SBI_EXT_0_1_REMOTE_SFENCE_VMA		0x6
#define SBI_EXT_0_1_REMOTE_SFENCE_VMA_ASID	0x7
#define SBI_EXT_0_1_SHUTDOWN			0x8
#define SBI_EXT_BASE				0x10
#define SBI_EXT_TIME				0x54494D45
#define SBI_EXT_IPI				0x735049
#define SBI_EXT_RFENCE				0x52464E43
#define SBI_EXT_HSM				0x48534D
#define SBI_EXT_SRST				0x53525354
#define SBI_EXT_PMU				0x504D55
#define SBI_EXT_DBCN				0x4442434E
#define SBI_EXT_SUSP				0x53555350
#define SBI_EXT_CPPC				0x43505043
#define SBI_EXT_DBTR				0x44425452
#define SBI_EXT_SSE				0x535345
#define SBI_EXT_FWFT				0x46574654

/* SBI function IDs for BASE extension*/
#define SBI_EXT_BASE_GET_SPEC_VERSION		0x0
#define SBI_EXT_BASE_GET_IMP_ID			0x1
#define SBI_EXT_BASE_GET_IMP_VERSION		0x2
#define SBI_EXT_BASE_PROBE_EXT			0x3
#define SBI_EXT_BASE_GET_MVENDORID		0x4
#define SBI_EXT_BASE_GET_MARCHID		0x5
#define SBI_EXT_BASE_GET_MIMPID			0x6

/* SBI function IDs for TIME extension*/
#define SBI_EXT_TIME_SET_TIMER			0x0

/* SBI function IDs for IPI extension*/
#define SBI_EXT_IPI_SEND_IPI			0x0

/* SBI function IDs for RFENCE extension*/
#define SBI_EXT_RFENCE_REMOTE_FENCE_I		0x0
#define SBI_EXT_RFENCE_REMOTE_SFENCE_VMA	0x1
#define SBI_EXT_RFENCE_REMOTE_SFENCE_VMA_ASID	0x2
#define SBI_EXT_RFENCE_REMOTE_HFENCE_GVMA_VMID	0x3
#define SBI_EXT_RFENCE_REMOTE_HFENCE_GVMA	0x4
#define SBI_EXT_RFENCE_REMOTE_HFENCE_VVMA_ASID	0x5
#define SBI_EXT_RFENCE_REMOTE_HFENCE_VVMA	0x6

/* SBI function IDs for HSM extension */
#define SBI_EXT_HSM_HART_START			0x0
#define SBI_EXT_HSM_HART_STOP			0x1
#define SBI_EXT_HSM_HART_GET_STATUS		0x2
#define SBI_EXT_HSM_HART_SUSPEND		0x3

#define SBI_HSM_STATE_STARTED			0x0
#define SBI_HSM_STATE_STOPPED			0x1
#define SBI_HSM_STATE_START_PENDING		0x2
#define SBI_HSM_STATE_STOP_PENDING		0x3
#define SBI_HSM_STATE_SUSPENDED			0x4
#define SBI_HSM_STATE_SUSPEND_PENDING		0x5
#define SBI_HSM_STATE_RESUME_PENDING		0x6

#define SBI_HSM_SUSP_BASE_MASK			0x7fffffff
#define SBI_HSM_SUSP_NON_RET_BIT		0x80000000
#define SBI_HSM_SUSP_PLAT_BASE			0x10000000

#define SBI_HSM_SUSPEND_RET_DEFAULT		0x00000000
#define SBI_HSM_SUSPEND_RET_PLATFORM		SBI_HSM_SUSP_PLAT_BASE
#define SBI_HSM_SUSPEND_RET_LAST		SBI_HSM_SUSP_BASE_MASK
#define SBI_HSM_SUSPEND_NON_RET_DEFAULT		SBI_HSM_SUSP_NON_RET_BIT
#define SBI_HSM_SUSPEND_NON_RET_PLATFORM	(SBI_HSM_SUSP_NON_RET_BIT | \
						 SBI_HSM_SUSP_PLAT_BASE)
#define SBI_HSM_SUSPEND_NON_RET_LAST		(SBI_HSM_SUSP_NON_RET_BIT | \
						 SBI_HSM_SUSP_BASE_MASK)

/* SBI function IDs for SRST extension */
#define SBI_EXT_SRST_RESET			0x0

#define SBI_SRST_RESET_TYPE_SHUTDOWN		0x0
#define SBI_SRST_RESET_TYPE_COLD_REBOOT	0x1
#define SBI_SRST_RESET_TYPE_WARM_REBOOT	0x2
#define SBI_SRST_RESET_TYPE_LAST	SBI_SRST_RESET_TYPE_WARM_REBOOT

#define SBI_SRST_RESET_REASON_NONE	0x0
#define SBI_SRST_RESET_REASON_SYSFAIL	0x1

/* SBI function IDs for PMU extension */
#define SBI_EXT_PMU_NUM_COUNTERS	0x0
#define SBI_EXT_PMU_COUNTER_GET_INFO	0x1
#define SBI_EXT_PMU_COUNTER_CFG_MATCH	0x2
#define SBI_EXT_PMU_COUNTER_START	0x3
#define SBI_EXT_PMU_COUNTER_STOP	0x4
#define SBI_EXT_PMU_COUNTER_FW_READ	0x5
#define SBI_EXT_PMU_COUNTER_FW_READ_HI	0x6
#define SBI_EXT_PMU_SNAPSHOT_SET_SHMEM	0x7

/* SBI function IDs for DBTR extension */
#define SBI_EXT_DBTR_NUM_TRIGGERS	0x0
#define SBI_EXT_DBTR_SETUP_SHMEM	0x1
#define SBI_EXT_DBTR_TRIGGER_READ	0x2
#define SBI_EXT_DBTR_TRIGGER_INSTALL	0x3
#define SBI_EXT_DBTR_TRIGGER_UPDATE	0x4
#define SBI_EXT_DBTR_TRIGGER_UNINSTALL	0x5
#define SBI_EXT_DBTR_TRIGGER_ENABLE	0x6
#define SBI_EXT_DBTR_TRIGGER_DISABLE	0x7

/* SBI function IDs for FW feature extension */
#define SBI_EXT_FWFT_SET		0x0
#define SBI_EXT_FWFT_GET		0x1

enum sbi_fwft_feature_t {
	SBI_FWFT_MISALIGNED_EXC_DELEG		= 0x0,
	SBI_FWFT_LANDING_PAD			= 0x1,
	SBI_FWFT_SHADOW_STACK			= 0x2,
	SBI_FWFT_DOUBLE_TRAP			= 0x3,
	SBI_FWFT_PTE_AD_HW_UPDATING		= 0x4,
	SBI_FWFT_POINTER_MASKING_PMLEN		= 0x5,
	SBI_FWFT_LOCAL_RESERVED_START		= 0x6,
	SBI_FWFT_LOCAL_RESERVED_END		= 0x3fffffff,
	SBI_FWFT_LOCAL_PLATFORM_START		= 0x40000000,
	SBI_FWFT_LOCAL_PLATFORM_END		= 0x7fffffff,

	SBI_FWFT_GLOBAL_RESERVED_START		= 0x80000000,
	SBI_FWFT_GLOBAL_RESERVED_END		= 0xbfffffff,
	SBI_FWFT_GLOBAL_PLATFORM_START		= 0xc0000000,
	SBI_FWFT_GLOBAL_PLATFORM_END		= 0xffffffff,
};

#define SBI_FWFT_GLOBAL_FEATURE_BIT		(1 << 31)
#define SBI_FWFT_PLATFORM_FEATURE_BIT		(1 << 30)

#define SBI_FWFT_SET_FLAG_LOCK			(1 << 0)

/** General pmu event codes specified in SBI PMU extension */
enum sbi_pmu_hw_generic_events_t {
	SBI_PMU_HW_NO_EVENT			= 0,
	SBI_PMU_HW_CPU_CYCLES			= 1,
	SBI_PMU_HW_INSTRUCTIONS			= 2,
	SBI_PMU_HW_CACHE_REFERENCES		= 3,
	SBI_PMU_HW_CACHE_MISSES			= 4,
	SBI_PMU_HW_BRANCH_INSTRUCTIONS		= 5,
	SBI_PMU_HW_BRANCH_MISSES		= 6,
	SBI_PMU_HW_BUS_CYCLES			= 7,
	SBI_PMU_HW_STALLED_CYCLES_FRONTEND	= 8,
	SBI_PMU_HW_STALLED_CYCLES_BACKEND	= 9,
	SBI_PMU_HW_REF_CPU_CYCLES		= 10,

	SBI_PMU_HW_GENERAL_MAX,
};

/**
 * Generalized hardware cache events:
 *
 *       { L1-D, L1-I, LLC, ITLB, DTLB, BPU, NODE } x
 *       { read, write, prefetch } x
 *       { accesses, misses }
 */
enum sbi_pmu_hw_cache_id {
	SBI_PMU_HW_CACHE_L1D		= 0,
	SBI_PMU_HW_CACHE_L1I		= 1,
	SBI_PMU_HW_CACHE_LL		= 2,
	SBI_PMU_HW_CACHE_DTLB		= 3,
	SBI_PMU_HW_CACHE_ITLB		= 4,
	SBI_PMU_HW_CACHE_BPU		= 5,
	SBI_PMU_HW_CACHE_NODE		= 6,

	SBI_PMU_HW_CACHE_MAX,
};

enum sbi_pmu_hw_cache_op_id {
	SBI_PMU_HW_CACHE_OP_READ	= 0,
	SBI_PMU_HW_CACHE_OP_WRITE	= 1,
	SBI_PMU_HW_CACHE_OP_PREFETCH	= 2,

	SBI_PMU_HW_CACHE_OP_MAX,
};

enum sbi_pmu_hw_cache_op_result_id {
	SBI_PMU_HW_CACHE_RESULT_ACCESS	= 0,
	SBI_PMU_HW_CACHE_RESULT_MISS	= 1,

	SBI_PMU_HW_CACHE_RESULT_MAX,
};

/**
 * Special "firmware" events provided by the OpenSBI, even if the hardware
 * does not support performance events. These events are encoded as a raw
 * event type in Linux kernel perf framework.
 */
enum sbi_pmu_fw_event_code_id {
	SBI_PMU_FW_MISALIGNED_LOAD	= 0,
	SBI_PMU_FW_MISALIGNED_STORE	= 1,
	SBI_PMU_FW_ACCESS_LOAD		= 2,
	SBI_PMU_FW_ACCESS_STORE		= 3,
	SBI_PMU_FW_ILLEGAL_INSN		= 4,
	SBI_PMU_FW_SET_TIMER		= 5,
	SBI_PMU_FW_IPI_SENT		= 6,
	SBI_PMU_FW_IPI_RECVD		= 7,
	SBI_PMU_FW_FENCE_I_SENT		= 8,
	SBI_PMU_FW_FENCE_I_RECVD	= 9,
	SBI_PMU_FW_SFENCE_VMA_SENT	= 10,
	SBI_PMU_FW_SFENCE_VMA_RCVD	= 11,
	SBI_PMU_FW_SFENCE_VMA_ASID_SENT	= 12,
	SBI_PMU_FW_SFENCE_VMA_ASID_RCVD	= 13,

	SBI_PMU_FW_HFENCE_GVMA_SENT	= 14,
	SBI_PMU_FW_HFENCE_GVMA_RCVD	= 15,
	SBI_PMU_FW_HFENCE_GVMA_VMID_SENT = 16,
	SBI_PMU_FW_HFENCE_GVMA_VMID_RCVD = 17,

	SBI_PMU_FW_HFENCE_VVMA_SENT	= 18,
	SBI_PMU_FW_HFENCE_VVMA_RCVD	= 19,
	SBI_PMU_FW_HFENCE_VVMA_ASID_SENT = 20,
	SBI_PMU_FW_HFENCE_VVMA_ASID_RCVD = 21,
	SBI_PMU_FW_MAX,
	/*
	 * Event codes 22 to 255 are reserved for future use.
	 * Event codes 256 to 65534 are reserved for SBI implementation
	 * specific custom firmware events.
	 */
	SBI_PMU_FW_RESERVED_MAX = 0xFFFE,
	/*
	 * Event code 0xFFFF is used for platform specific firmware
	 * events where the event data contains any event specific information.
	 */
	SBI_PMU_FW_PLATFORM = 0xFFFF,
};

/** SBI PMU event idx type */
enum sbi_pmu_event_type_id {
	SBI_PMU_EVENT_TYPE_HW				= 0x0,
	SBI_PMU_EVENT_TYPE_HW_CACHE			= 0x1,
	SBI_PMU_EVENT_TYPE_HW_RAW			= 0x2,
	SBI_PMU_EVENT_TYPE_FW				= 0xf,
	SBI_PMU_EVENT_TYPE_MAX,
};

/** SBI PMU counter type */
enum sbi_pmu_ctr_type {
	SBI_PMU_CTR_TYPE_HW = 0,
	SBI_PMU_CTR_TYPE_FW,
};

/* Helper macros to decode event idx */
#define SBI_PMU_EVENT_IDX_MASK 0xFFFFF
#define SBI_PMU_EVENT_IDX_TYPE_OFFSET 16
#define SBI_PMU_EVENT_IDX_TYPE_MASK (0xF << SBI_PMU_EVENT_IDX_TYPE_OFFSET)
#define SBI_PMU_EVENT_IDX_CODE_MASK 0xFFFF
#define SBI_PMU_EVENT_RAW_IDX 0x20000

#define SBI_PMU_EVENT_IDX_INVALID 0xFFFFFFFF

#define SBI_PMU_EVENT_HW_CACHE_OPS_RESULT	0x1
#define SBI_PMU_EVENT_HW_CACHE_OPS_ID_MASK	0x6
#define SBI_PMU_EVENT_HW_CACHE_OPS_ID_OFFSET	1
#define SBI_PMU_EVENT_HW_CACHE_ID_MASK		0xfff8
#define SBI_PMU_EVENT_HW_CACHE_ID_OFFSET	3

/* Flags defined for config matching function */
#define SBI_PMU_CFG_FLAG_SKIP_MATCH	(1 << 0)
#define SBI_PMU_CFG_FLAG_CLEAR_VALUE	(1 << 1)
#define SBI_PMU_CFG_FLAG_AUTO_START	(1 << 2)
#define SBI_PMU_CFG_FLAG_SET_VUINH	(1 << 3)
#define SBI_PMU_CFG_FLAG_SET_VSINH	(1 << 4)
#define SBI_PMU_CFG_FLAG_SET_UINH	(1 << 5)
#define SBI_PMU_CFG_FLAG_SET_SINH	(1 << 6)
#define SBI_PMU_CFG_FLAG_SET_MINH	(1 << 7)

/* Flags defined for counter start function */
#define SBI_PMU_START_FLAG_SET_INIT_VALUE (1 << 0)
#define SBI_PMU_START_FLAG_INIT_FROM_SNAPSHOT (1 << 1)

/* Flags defined for counter stop function */
#define SBI_PMU_STOP_FLAG_RESET (1 << 0)
#define SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT (1 << 1)

/* SBI function IDs for DBCN extension */
#define SBI_EXT_DBCN_CONSOLE_WRITE		0x0
#define SBI_EXT_DBCN_CONSOLE_READ		0x1
#define SBI_EXT_DBCN_CONSOLE_WRITE_BYTE		0x2

/* SBI function IDs for SUSP extension */
#define SBI_EXT_SUSP_SUSPEND			0x0

#define SBI_SUSP_SLEEP_TYPE_SUSPEND		0x0
#define SBI_SUSP_SLEEP_TYPE_LAST		SBI_SUSP_SLEEP_TYPE_SUSPEND
#define SBI_SUSP_PLATFORM_SLEEP_START		0x80000000

/* SBI function IDs for CPPC extension */
#define SBI_EXT_CPPC_PROBE			0x0
#define SBI_EXT_CPPC_READ			0x1
#define SBI_EXT_CPPC_READ_HI			0x2
#define SBI_EXT_CPPC_WRITE			0x3

enum sbi_cppc_reg_id {
	SBI_CPPC_HIGHEST_PERF		= 0x00000000,
	SBI_CPPC_NOMINAL_PERF		= 0x00000001,
	SBI_CPPC_LOW_NON_LINEAR_PERF	= 0x00000002,
	SBI_CPPC_LOWEST_PERF		= 0x00000003,
	SBI_CPPC_GUARANTEED_PERF	= 0x00000004,
	SBI_CPPC_DESIRED_PERF		= 0x00000005,
	SBI_CPPC_MIN_PERF		= 0x00000006,
	SBI_CPPC_MAX_PERF		= 0x00000007,
	SBI_CPPC_PERF_REDUC_TOLERANCE	= 0x00000008,
	SBI_CPPC_TIME_WINDOW		= 0x00000009,
	SBI_CPPC_CTR_WRAP_TIME		= 0x0000000A,
	SBI_CPPC_REFERENCE_CTR		= 0x0000000B,
	SBI_CPPC_DELIVERED_CTR		= 0x0000000C,
	SBI_CPPC_PERF_LIMITED		= 0x0000000D,
	SBI_CPPC_ENABLE			= 0x0000000E,
	SBI_CPPC_AUTO_SEL_ENABLE	= 0x0000000F,
	SBI_CPPC_AUTO_ACT_WINDOW	= 0x00000010,
	SBI_CPPC_ENERGY_PERF_PREFERENCE	= 0x00000011,
	SBI_CPPC_REFERENCE_PERF		= 0x00000012,
	SBI_CPPC_LOWEST_FREQ		= 0x00000013,
	SBI_CPPC_NOMINAL_FREQ		= 0x00000014,
	SBI_CPPC_ACPI_LAST		= SBI_CPPC_NOMINAL_FREQ,
	SBI_CPPC_TRANSITION_LATENCY	= 0x80000000,
	SBI_CPPC_NON_ACPI_LAST		= SBI_CPPC_TRANSITION_LATENCY,
};

/* SBI Function IDs for SSE extension */
#define SBI_EXT_SSE_READ_ATTR		0x00000000
#define SBI_EXT_SSE_WRITE_ATTR		0x00000001
#define SBI_EXT_SSE_REGISTER		0x00000002
#define SBI_EXT_SSE_UNREGISTER		0x00000003
#define SBI_EXT_SSE_ENABLE		0x00000004
#define SBI_EXT_SSE_DISABLE		0x00000005
#define SBI_EXT_SSE_COMPLETE		0x00000006
#define SBI_EXT_SSE_INJECT		0x00000007

/* SBI SSE Event Attributes. */
enum sbi_sse_attr_id {
	SBI_SSE_ATTR_STATUS		= 0x00000000,
	SBI_SSE_ATTR_PRIO		= 0x00000001,
	SBI_SSE_ATTR_CONFIG		= 0x00000002,
	SBI_SSE_ATTR_PREFERRED_HART	= 0x00000003,
	SBI_SSE_ATTR_ENTRY_PC		= 0x00000004,
	SBI_SSE_ATTR_ENTRY_ARG		= 0x00000005,
	SBI_SSE_ATTR_INTERRUPTED_SEPC	= 0x00000006,
	SBI_SSE_ATTR_INTERRUPTED_FLAGS	= 0x00000007,
	SBI_SSE_ATTR_INTERRUPTED_A6	= 0x00000008,
	SBI_SSE_ATTR_INTERRUPTED_A7	= 0x00000009,

	SBI_SSE_ATTR_MAX		= 0x0000000A
};

#define SBI_SSE_ATTR_STATUS_STATE_OFFSET	0
#define SBI_SSE_ATTR_STATUS_STATE_MASK		0x3
#define SBI_SSE_ATTR_STATUS_PENDING_OFFSET	2
#define SBI_SSE_ATTR_STATUS_INJECT_OFFSET	3

#define SBI_SSE_ATTR_CONFIG_ONESHOT	(1 << 0)

#define SBI_SSE_ATTR_INTERRUPTED_FLAGS_STATUS_SPP	BIT(0)
#define SBI_SSE_ATTR_INTERRUPTED_FLAGS_STATUS_SPIE	BIT(1)
#define SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPV	BIT(2)
#define SBI_SSE_ATTR_INTERRUPTED_FLAGS_HSTATUS_SPVP	BIT(3)

enum sbi_sse_state {
	SBI_SSE_STATE_UNUSED		= 0,
	SBI_SSE_STATE_REGISTERED	= 1,
	SBI_SSE_STATE_ENABLED		= 2,
	SBI_SSE_STATE_RUNNING		= 3,
};

/* SBI SSE Event IDs. */
#define SBI_SSE_EVENT_LOCAL_RAS			0x00000000
#define SBI_SSE_EVENT_LOCAL_PLAT_0_START	0x00004000
#define SBI_SSE_EVENT_LOCAL_PLAT_0_END		0x00007fff
#define SBI_SSE_EVENT_GLOBAL_RAS		0x00008000
#define SBI_SSE_EVENT_GLOBAL_PLAT_0_START	0x00004000
#define SBI_SSE_EVENT_GLOBAL_PLAT_0_END		0x00007fff

#define SBI_SSE_EVENT_LOCAL_PMU			0x00010000
#define SBI_SSE_EVENT_LOCAL_PLAT_1_START	0x00014000
#define SBI_SSE_EVENT_LOCAL_PLAT_1_END		0x00017fff
#define SBI_SSE_EVENT_GLOBAL_PLAT_1_START	0x0001c000
#define SBI_SSE_EVENT_GLOBAL_PLAT_1_END		0x0001ffff

#define SBI_SSE_EVENT_LOCAL_PLAT_2_START	0x00024000
#define SBI_SSE_EVENT_LOCAL_PLAT_2_END		0x00027fff
#define SBI_SSE_EVENT_GLOBAL_PLAT_2_START	0x0002c000
#define SBI_SSE_EVENT_GLOBAL_PLAT_2_END		0x0002ffff

#define SBI_SSE_EVENT_LOCAL_SOFTWARE		0xffff0000
#define SBI_SSE_EVENT_LOCAL_PLAT_3_START	0xffff4000
#define SBI_SSE_EVENT_LOCAL_PLAT_3_END		0xffff7fff
#define SBI_SSE_EVENT_GLOBAL_SOFTWARE		0xffff8000
#define SBI_SSE_EVENT_GLOBAL_PLAT_3_START	0xffffc000
#define SBI_SSE_EVENT_GLOBAL_PLAT_3_END		0xffffffff

#define SBI_SSE_EVENT_GLOBAL_BIT		(1 << 15)
#define SBI_SSE_EVENT_PLATFORM_BIT		(1 << 14)

/* SBI base specification related macros */
#define SBI_SPEC_VERSION_MAJOR_OFFSET		24
#define SBI_SPEC_VERSION_MAJOR_MASK		0x7f
#define SBI_SPEC_VERSION_MINOR_MASK		0xffffff
#define SBI_EXT_VENDOR_START			0x09000000
#define SBI_EXT_VENDOR_END			0x09FFFFFF
#define SBI_EXT_FIRMWARE_START			0x0A000000
#define SBI_EXT_FIRMWARE_END			0x0AFFFFFF

/* SBI return error codes */
#define SBI_SUCCESS				0
#define SBI_ERR_FAILED				-1
#define SBI_ERR_NOT_SUPPORTED			-2
#define SBI_ERR_INVALID_PARAM			-3
#define SBI_ERR_DENIED				-4
#define SBI_ERR_INVALID_ADDRESS			-5
#define SBI_ERR_ALREADY_AVAILABLE		-6
#define SBI_ERR_ALREADY_STARTED			-7
#define SBI_ERR_ALREADY_STOPPED			-8
#define SBI_ERR_NO_SHMEM			-9
#define SBI_ERR_INVALID_STATE			-10
#define SBI_ERR_BAD_RANGE			-11
#define SBI_ERR_TIMEOUT       -12
#define SBI_ERR_IO            -13

#define SBI_LAST_ERR				SBI_ERR_BAD_RANGE

#endif