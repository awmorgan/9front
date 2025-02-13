#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "io.h"
#include "../port/audioif.h"
#include "../port/error.h"

typedef struct Ctlr Ctlr;
typedef struct Ring Ring;

enum {
	Byteps = 4,
	Wmark = 8,

	PARAM = 0x04/4,
	TCSR = 0x08/4,
		TCSR_TE = 1<<31,
		TCSR_FR = 1<<25,
		TCSR_SR = 1<<24,
		TCSR_FEF = 1<<18, /* w1c fifo error */
		TCSR_FWF = 1<<17,
		TCSR_FRF = 1<<16, /* watermark hit */
		TCSR_FEIE = 1<<10,
		TCSR_FRIE = 1<<8,
	TCR1 = 0x0c/4,
	TCR2 = 0x10/4,
		TCR2_BCP = 1<<25,
		TCR2_BCD_SLAVE = 0<<24,
	TCR3 = 0x14/4,
		TCR3_TCE_SHIFT = 16,
	TCR4 = 0x18/4,
		TCR4_FCONT = 1<<28,
		TCR4_FPACK_16BIT = 3<<24,
		TCR4_FRSZ_SHIFT = 16,
		TCR4_SYWD_SHIFT = 8,
		TCR4_CHMOD = 1<<5,
		TCR4_MSB_FIRST = 1<<4,
		TCR4_FSE = 1<<3,
		TCR4_FSP_LOW = 1<<1,
		TCR4_FSD_SLAVE = 0<<0,
	TCR5 = 0x1c/4,
		TCR5_WNW_SHIFT = 24,
		TCR5_W0W_SHIFT = 16,
		TCR5_FBT_SHIFT = 8,
	TDR0 = 0x20/4,
	TFR0 = 0x40/4,
		TFRx_WFP_SHIFT = 16,
		TFRx_RFP_SHIFT = 0,
	TMR = 0x60/4,
};

struct Ring {
	Rendez r;

	uchar *buf;
	ulong nbuf;

	ulong ri;
	ulong wi;
};

struct Ctlr {
	u32int *reg;
	Audio *adev;
	int fifosz;
	int hp;

	Ring w;
	int wactive;

	Lock;
};

#define wr(a, v) ctlr->reg[a] = v
#define rd(a) ctlr->reg[a]

static long
buffered(Ring *r)
{
	ulong ri, wi;

	ri = r->ri;
	wi = r->wi;
	if(wi >= ri)
		return wi - ri;
	else
		return r->nbuf - (ri - wi);
}

static long
available(Ring *r)
{
	long m;

	m = (r->nbuf - Byteps) - buffered(r);
	if(m < 0)
		m = 0;
	return m;
}

static long
writering(Ring *r, uchar *p, long n)
{
	long n0, m;

	n0 = n;
	while(n > 0){
		if((m = available(r)) <= 0)
			break;
		if(m > n)
			m = n;
		if(p){
			if(r->wi + m > r->nbuf)
				m = r->nbuf - r->wi;
			memmove(r->buf + r->wi, p, m);
			p += m;
		}
		r->wi = (r->wi + m) % r->nbuf;
		n -= m;
	}
	return n0 - n;
}

static int
outavail(void *arg)
{
	Ring *r = arg;

	return available(r) > 0;
}

static int
outrate(void *arg)
{
	Ctlr *ctlr = arg;
	int delay = ctlr->adev->delay*Byteps;

	return delay <= 0 || buffered(&ctlr->w) <= delay || ctlr->wactive == 0;
}

static void
saikick(Ctlr *ctlr)
{
	int delay;

	delay = ctlr->adev->delay*Byteps;
	if(buffered(&ctlr->w) >= delay){
		ctlr->wactive = 1;
		/* activate channel 1 */
		wr(TCR3, 1<<TCR3_TCE_SHIFT);
		wr(TCSR, TCSR_TE | TCSR_FEF | TCSR_FRIE | TCSR_FEIE);
	}
}

static void
setempty(Ctlr *ctlr)
{
	ilock(ctlr);
	ctlr->w.ri = 0;
	ctlr->w.wi = 0;
	iunlock(ctlr);
}

static void
saistop(Ctlr *ctlr)
{
	if(!ctlr->wactive)
		return;
	ctlr->wactive = 0;
	wr(TCSR, TCSR_FR | TCSR_SR);
}

static int
inactive(void *arg)
{
	Ctlr *ctlr = arg;
	return !ctlr->wactive;
}

static long
saiwrite(Audio *adev, void *a, long n, vlong)
{
	Ctlr *ctlr = adev->ctlr;
	uchar *p, *e;
	Ring *r;

	p = a;
	e = p + n;
	r = &ctlr->w;
	while(p < e){
		if((n = writering(r, p, e - p)) <= 0){
			saikick(ctlr);
			sleep(&r->r, outavail, r);
			continue;
		}
		p += n;
	}
	saikick(ctlr);
	while(outrate(ctlr) == 0)
		sleep(&r->r, outrate, ctlr);
	return p - (uchar*)a;
}

static void
saiclose(Audio *adev, int mode)
{
	Ctlr *ctlr = adev->ctlr;

	if(mode == OWRITE || mode == ORDWR){
		sleep(&ctlr->w.r, inactive, ctlr);
		setempty(ctlr);
	}
}

static void
saireset(Ctlr *ctlr)
{
	/* fifo+software reset */
	wr(TCSR, TCSR_FR | TCSR_SR);
	delay(1);
	wr(TCSR, 0);
	delay(1);

	/* watermark - hit early enough */
	wr(TCR1, Wmark);
	/* slave bitclock (active low) */
	wr(TCR2, TCR2_BCD_SLAVE | TCR2_BCP);
	/* activate channel 1 */
	wr(TCR3, 1<<TCR3_TCE_SHIFT);
	/* set up for i²s */
	wr(TCR4,
		TCR4_FCONT | /* continue on errors */
		TCR4_CHMOD | /* output mode, no TDM */
		TCR4_MSB_FIRST |
		TCR4_FPACK_16BIT | /* 16-bit packed words */
		1<<TCR4_FRSZ_SHIFT | /* two words per frame */
		15<<TCR4_SYWD_SHIFT | /* frame sync per word */
		/* frame sync */
		TCR4_FSE | /* one bit earlier */
		TCR4_FSP_LOW | /* active high */
		TCR4_FSD_SLAVE /* generate internally */
	);
	/* 16-bit words, MSB first */
	wr(TCR5, 15<<TCR5_WNW_SHIFT | 15<<TCR5_W0W_SHIFT | 15<<TCR5_FBT_SHIFT);
	/* mask all but first two words */
	wr(TMR, ~3UL);
}

static long
saictl(Audio *adev, void *a, long n, vlong)
{
	char *p, *e, *x, *tok[4];
	Ctlr *ctlr = adev->ctlr;
	int ntok;

	p = a;
	e = p + n;
	for(; p < e; p = x){
		if(x = strchr(p, '\n'))
			*x++ = 0;
		else
			x = e;
		ntok = tokenize(p, tok, 4);
		if(ntok <= 0)
			continue;

		if(cistrcmp(tok[0], "reset") == 0)
			saireset(ctlr);
		else
			error(Ebadctl);
	}
	return n;
}

static long
fifo(Ctlr *ctlr, long n)
{
	long n0, m;
	u32int *p;
	Ring *r;

	n0 = n;
	r = &ctlr->w;
	while(n > 0){
		if((m = buffered(r)) <= 0 || m < 4)
			break;
		if(m > n)
			m = n;

		if(r->ri + m > r->nbuf)
			m = r->nbuf - r->ri;
		m &= ~(Byteps-1);
		for(p = (u32int*)(r->buf + r->ri); p < (u32int*)(r->buf + r->ri + m); p++)
			wr(TDR0, *p);

		r->ri = (r->ri + m) % r->nbuf;
		n -= m;
	}
	return n0 - n;
}

static void
saiinterrupt(Ureg *, void *arg)
{
	Ctlr *ctlr = arg;
	u32int v;
	Ring *r;

	ilock(ctlr);
	v = rd(TCSR);
	if(v & (TCSR_FEF | TCSR_FRF | TCSR_FWF)){
		r = &ctlr->w;
		if(ctlr->wactive){
			if(buffered(r) < ctlr->fifosz*Byteps) /* having less than fifo buffered */
				saistop(ctlr);
			else if(fifo(ctlr, (ctlr->fifosz-Wmark)*Byteps) > 0)
				wr(TCSR, v | TCSR_TE | TCSR_FEF);
		}
		wakeup(&r->r);
	}
	iunlock(ctlr);
}

static long
saistatus(Audio *adev, void *a, long n, vlong)
{
	Ctlr *ctlr = adev->ctlr;
	u32int v, p;
	char *s, *e;

	s = a;
	e = s + n;
	v = rd(TCSR);
	p = rd(TFR0);
	s = seprint(s, e, "transmit wfp %d rfp %d delay %d buf %ld avail %ld active %d%s%s%s%s\n",
		(p>>TFRx_WFP_SHIFT) & 0xff,
		(p>>TFRx_RFP_SHIFT) & 0xff,
		adev->delay,
		buffered(&ctlr->w),
		available(&ctlr->w),
		ctlr->wactive,
		(v & TCSR_TE) ? " enabled" : "",
		(v & TCSR_FEF) ? " fifo_error" : "",
		(v & TCSR_FWF) ? " fifo_warn" : "",
		(v & TCSR_FRF) ? " fifo_req" : ""
	);
	s = seprint(s, e, "hp %d\n", ctlr->hp);

	return s - (char*)a;
}

static long
saibuffered(Audio *adev)
{
	Ctlr *ctlr = adev->ctlr;

	return buffered(&ctlr->w);
}

static void
jacksense(uint pin, void *a)
{
	Ctlr *ctlr = a;

	ctlr->hp = gpioin(pin);
}

static int
saiprobe(Audio *adev)
{
	Ctlr *ctlr;

	if(adev->ctlrno > 0)
		return -1;

	ctlr = mallocz(sizeof(Ctlr), 1);
	if(ctlr == nil)
		return -1;

	ctlr->w.buf = malloc(ctlr->w.nbuf = 44100*Byteps*2);
	ctlr->reg = (u32int*)(VIRTIO + 0x8b0000);
	ctlr->adev = adev;
	ctlr->fifosz = 1 << ((rd(PARAM)>>8) & 0xf);

	adev->delay = 2048;
	adev->ctlr = ctlr;
	adev->write = saiwrite;
	adev->close = saiclose;
	adev->buffered = saibuffered;
	adev->status = saistatus;
	adev->ctl = saictl;

	iomuxpad("pad_sai2_rxfs", "gpio4_io21", "SION ~LVTTL HYS ~PUE ~ODE FAST 45_OHM");
	iomuxpad("pad_sai2_rxc", "sai2_rx_bclk", "~LVTTL HYS PUE ~ODE FAST 45_OHM");
	iomuxpad("pad_sai2_rxd0", "sai2_rx_data0", "~LVTTL HYS PUE ~ODE FAST 45_OHM");
	iomuxpad("pad_sai2_txfs", "sai2_tx_sync", "~LVTTL HYS PUE ~ODE FAST 45_OHM");
	iomuxpad("pad_sai2_txc", "sai2_tx_bclk", "~LVTTL HYS PUE ~ODE FAST 45_OHM");
	iomuxpad("pad_sai2_txd0", "sai2_tx_data0", "~LVTTL HYS PUE ~ODE FAST 45_OHM");
	iomuxpad("pad_sai2_mclk", "sai2_mclk", "~LVTTL HYS PUE ~ODE FAST 45_OHM");

	setclkrate("sai2.ipg_clk", "osc_25m_ref_clk", 25*Mhz);
	setclkgate("sai2.ipg_clk", 1);

	intrenable(IRQsai2, saiinterrupt, ctlr, BUSUNKNOWN, "sai2");
	ctlr->hp = gpioin(GPIO_PIN(4, 21));
	gpiointrenable(GPIO_PIN(4, 21), GpioEdge, jacksense, ctlr);
	saireset(ctlr);

	return 0;
}

void
sailink(void)
{
	addaudiocard("sai", saiprobe);
}
