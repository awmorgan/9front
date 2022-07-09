#define MIDR_EL1			SYSREG(3,0,0,0,0)
#define MPIDR_EL1			SYSREG(3,0,0,0,5)
#define ID_AA64AFR0_EL1			SYSREG(3,0,0,5,4)
#define ID_AA64AFR1_EL1			SYSREG(3,0,0,5,5)
#define ID_AA64DFR0_EL1			SYSREG(3,0,0,5,0)
#define ID_AA64DFR1_EL1			SYSREG(3,0,0,5,1)
#define ID_AA64ISAR0_EL1		SYSREG(3,0,0,6,0)
#define ID_AA64ISAR1_EL1		SYSREG(3,0,0,6,1)
#define ID_AA64MMFR0_EL1		SYSREG(3,0,0,7,0)
#define ID_AA64MMFR1_EL1		SYSREG(3,0,0,7,1)
#define ID_AA64PFR0_EL1			SYSREG(3,0,0,4,0)
#define ID_AA64PFR1_EL1			SYSREG(3,0,0,4,1)
#define SCTLR_EL1			SYSREG(3,0,1,0,0)
#define CPACR_EL1			SYSREG(3,0,1,0,2)
#define MAIR_EL1			SYSREG(3,0,10,2,0)
#define TCR_EL1				SYSREG(3,0,2,0,2)
#define TTBR0_EL1			SYSREG(3,0,2,0,0)
#define TTBR1_EL1			SYSREG(3,0,2,0,1)
#define ESR_EL1				SYSREG(3,0,5,2,0)
#define FAR_EL1				SYSREG(3,0,6,0,0)
#define VBAR_EL1			SYSREG(3,0,12,0,0)
#define VTTBR_EL2			SYSREG(3,4,2,1,0)
#define SP_EL0				SYSREG(3,0,4,1,0)
#define SP_EL1				SYSREG(3,4,4,1,0)
#define SP_EL2				SYSREG(3,6,4,1,0)
#define SCTLR_EL2			SYSREG(3,4,1,0,0)
#define HCR_EL2				SYSREG(3,4,1,1,0)
#define MDCR_EL2			SYSREG(3,4,1,1,1)
#define PMCR_EL0			SYSREG(3,3,9,12,0)
#define PMCNTENSET			SYSREG(3,3,9,12,1)
#define PMCCNTR_EL0			SYSREG(3,3,9,13,0)
#define PMUSERENR_EL0			SYSREG(3,3,9,14,0)

#define CNTPCT_EL0			SYSREG(3,3,14,0,1)
#define CNTVCT_EL0			SYSREG(3,3,14,0,2)
#define CNTKCTL_EL1			SYSREG(3,0,14,1,0)
#define	CNTFRQ_EL0			SYSREG(3,3,14,0,0)
#define CNTP_TVAL_EL0			SYSREG(3,3,14,2,0)
#define CNTP_CTL_EL0			SYSREG(3,3,14,2,1)
#define CNTP_CVAL_EL0			SYSREG(3,3,14,2,2)
#define CNTVOFF_EL2			SYSREG(3,4,14,0,3)

#define TPIDR_EL0			SYSREG(3,3,13,0,2)
#define TPIDR_EL1			SYSREG(3,0,13,0,4)

#define CCSIDR_EL1			SYSREG(3,1,0,0,0)
#define CSSELR_EL1			SYSREG(3,2,0,0,0)

#define ACTLR_EL2			SYSREG(3,4,1,0,1)
#define CPUACTLR_EL1			SYSREG(3,1,15,2,0)
#define CPUECTLR_EL1			SYSREG(3,1,15,2,1)
#define CBAR_EL1			SYSREG(3,1,15,3,0)

#define	ICC_AP0R_EL1(m)			SYSREG(3,0,12,8,4|(m))
#define	ICC_AP1R_EL1(m)			SYSREG(3,0,12,9,0|(m))
#define	ICC_ASGI1R_EL1			SYSREG(3,0,12,11,6)
#define	ICC_BPR0_EL1			SYSREG(3,0,12,8,3)
#define	ICC_BPR1_EL1			SYSREG(3,0,12,12,3)
#define	ICC_CTLR_EL1			SYSREG(3,0,12,12,4)
#define	ICC_DIR_EL1			SYSREG(3,0,12,11,1)
#define	ICC_EOIR0_EL1			SYSREG(3,0,12,8,1)
#define	ICC_EOIR1_EL1			SYSREG(3,0,12,12,1)
#define	ICC_HPPIR0_EL1			SYSREG(3,0,12,8,2)
#define	ICC_HPPIR1_EL1			SYSREG(3,0,12,12,2)
#define ICC_IAR0_EL1			SYSREG(3,0,12,8,0)
#define	ICC_IAR1_EL1			SYSREG(3,0,12,12,0)
#define	ICC_IGRPEN0_EL1			SYSREG(3,0,12,12,6)
#define	ICC_IGRPEN1_EL1			SYSREG(3,0,12,12,7)
#define	ICC_NMIAR1_EL1			SYSREG(3,0,12,9,5)
#define	ICC_PMR_EL1			SYSREG(3,0,4,6,0)
#define	ICC_RPR_EL1			SYSREG(3,0,12,11,3)
#define	ICC_SGI0R_EL1			SYSREG(3,0,12,11,7)
#define	ICC_SGI1R_EL1			SYSREG(3,0,12,11,5)
#define ICC_SRE_EL1			SYSREG(3,0,12,12,5)

/* l.s redefines this for the assembler */
#define SYSREG(op0,op1,Cn,Cm,op2)	((op0)<<19|(op1)<<16|(Cn)<<12|(Cm)<<8|(op2)<<5)

#define	OSHLD	(0<<2 | 1)
#define OSHST	(0<<2 | 2)
#define	OSH	(0<<2 | 3)
#define NSHLD	(1<<2 | 1)
#define NSHST	(1<<2 | 2)
#define NSH	(1<<2 | 3)
#define ISHLD	(2<<2 | 1)
#define ISHST	(2<<2 | 2)
#define ISH	(2<<2 | 3)
#define LD	(3<<2 | 1)
#define ST	(3<<2 | 2)
#define SY	(3<<2 | 3)
