void		_assert(char*);
void		accounttime(void);
Timer*		addclock0link(void (*)(void), int);
Physseg*	addphysseg(Physseg*);
void		addbootfile(char*, uchar*, ulong);
void		addwatchdog(Watchdog*);
Block*		adjustblock(Block*, int);
void		alarmkproc(void*);
Block*		allocb(int);
int		anyhigher(void);
int		anyready(void);
Image*		attachimage(int, Chan*, uintptr, ulong);
ulong		beswal(ulong);
uvlong		beswav(uvlong);
int		blocklen(Block*);
void		bootlinks(void);
void		cachedel(Image*, uintptr);
void		cachepage(Page*, Image*);
void		callwithureg(void(*)(Ureg*));
char*		chanpath(Chan*);
int		canlock(Lock*);
int		canpage(Proc*);
int		canqlock(QLock*);
int		canrlock(RWlock*);
void		chandevinit(void);
void		chandevreset(void);
void		chandevshutdown(void);
void		chanfree(Chan*);
void		checkalarms(void);
void		checkb(Block*, char*);
void		cinit(void);
Chan*		cclone(Chan*);
void		cclose(Chan*);
void		ccloseq(Chan*);
void		closeegrp(Egrp*);
void		closefgrp(Fgrp*);
void		closepgrp(Pgrp*);
void		closergrp(Rgrp*);
long		clrfpintr(void);
void		cmderror(Cmdbuf*, char*);
int		cmount(Chan*, Chan*, int, char*);
void		confinit(void);
int		consactive(void);
void		(*consdebug)(void);
void		cpushutdown(void);
int		copen(Chan*);
void		cclunk(Chan*);
Block*		concatblock(Block*);
Block*		copyblock(Block*, int);
void		copypage(Page*, Page*);
void		countpagerefs(ulong*, int);
int		cread(Chan*, uchar*, int, vlong);
void		ctrunc(Chan*);
void		cunmount(Chan*, Chan*);
void		cupdate(Chan*, uchar*, int, vlong);
void		cwrite(Chan*, uchar*, int, vlong);
uintptr		dbgpc(Proc*);
long		decref(Ref*);
int		decrypt(void*, void*, int);
void		delay(int);
Proc*		dequeueproc(Schedq*, Proc*);
Chan*		devattach(int, char*);
Block*		devbread(Chan*, long, ulong);
long		devbwrite(Chan*, Block*, ulong);
Chan*		devclone(Chan*);
int		devconfig(int, char *, DevConf *);
Chan*		devcreate(Chan*, char*, int, ulong);
void		devdir(Chan*, Qid, char*, vlong, char*, long, Dir*);
long		devdirread(Chan*, char*, long, Dirtab*, int, Devgen*);
Devgen		devgen;
void		devinit(void);
int		devno(int, int);
Chan*		devopen(Chan*, int, Dirtab*, int, Devgen*);
void		devpermcheck(char*, ulong, int);
void		devpower(int);
void		devremove(Chan*);
void		devreset(void);
void		devshutdown(void);
int		devstat(Chan*, uchar*, int, Dirtab*, int, Devgen*);
Walkqid*	devwalk(Chan*, Chan*, char**, int, Dirtab*, int, Devgen*);
int		devwstat(Chan*, uchar*, int);
Dir*		dirchanstat(Chan *);
void		drawactive(int);
void		drawcmap(void);
void		dtracytick(Ureg*);
void		dumpaproc(Proc*);
void		dumpregs(Ureg*);
void		dumpstack(void);
Fgrp*		dupfgrp(Fgrp*);
void		dupswap(Page*);
void		edfinit(Proc*);
char*		edfadmit(Proc*);
int		edfready(Proc*);
void		edfrecord(Proc*);
void		edfrun(Proc*, int);
void		edfstop(Proc*);
void		edfyield(void);
int		emptystr(char*);
int		encrypt(void*, void*, int);
void		envcpy(Egrp*, Egrp*);
int		eqchan(Chan*, Chan*, int);
int		eqchantdqid(Chan*, int, int, Qid, int);
int		eqqid(Qid, Qid);
void		error(char*);
void		eqlock(QLock*);
uintptr		execregs(uintptr, ulong, ulong);
void		exhausted(char*);
void		exit(int);
uvlong		fastticks(uvlong*);
uvlong		fastticks2ns(uvlong);
uvlong		fastticks2us(uvlong);
int		fault(uintptr, uintptr, int);
void		fdclose(int, int);
Chan*		fdtochan(int, int, int, int);
int		findmount(Chan**, Mhead**, int, int, Qid);
void		flushmmu(void);
void		forceclosefgrp(void);
void		forkchild(Proc*, Ureg*);
void		forkret(void);
void		free(void*);
void		freeb(Block*);
void		freeblist(Block*);
int		freebroken(void);
void		freenote(Note*);
void		freenotes(Proc*);
void		freepages(Page*, Page*, ulong);
void		freepte(Segment*, Pte*);
void		getcolor(ulong, ulong*, ulong*, ulong*);
uintptr		getmalloctag(void*);
uintptr		getrealloctag(void*);
void		gotolabel(Label*);
char*		getconfenv(void);
long		hostdomainwrite(char*, int);
long		hostownerwrite(char*, int);
void 		(*hwrandbuf)(void*, ulong);
void		hzsched(void);
Block*		iallocb(int);
uintptr		ibrk(uintptr, int);
void		ilock(Lock*);
void		interrupted(void);
void		iunlock(Lock*);
ulong		imagecached(void);
ulong		imagereclaim(int);
long		incref(Ref*);
void		init0(void);
void		initseg(void);
int		ioalloc(ulong, ulong, ulong, char*);
void		iofree(ulong);
void		iomapinit(ulong);
int		ioreserve(ulong, ulong, ulong, char*);
int		ioreservewin(ulong, ulong, ulong, ulong, char*);
int		iounused(ulong, ulong);
int		iprint(char*, ...);
void		isdir(Chan*);
int		iseve(void);
int		islo(void);
Segment*	isoverlap(uintptr, uintptr);
Physseg*	findphysseg(char*);
int		kenter(Ureg*);
void		kexit(Ureg*);
void		kickpager(void);
void		killbig(char*);
void		kproc(char*, void(*)(void*), void*);
void		kprocchild(Proc*, void (*)(void));
void		(*kproftimer)(uintptr);
void		ksetenv(char*, char*, int);
void		kstrcpy(char*, char*, int);
void		kstrdup(char**, char*);
int		lock(Lock*);
void		logopen(Log*);
void		logclose(Log*);
char*		logctl(Log*, int, char**, Logflag*);
void		logn(Log*, int, void*, int);
long		logread(Log*, void*, ulong, long);
void		log(Log*, int, char*, ...);
Cmdtab*		lookupcmd(Cmdbuf*, Cmdtab*, int);
Page*		lookpage(Image*, uintptr);
#define		MS2NS(n) (((vlong)(n))*1000000LL)
void		machinit(void);
void*		mallocz(ulong, int);
void*		malloc(ulong);
void*		mallocalign(ulong, ulong, long, ulong);
void		mallocsummary(void);
void		memmapdump(void);
uvlong		memmapnext(uvlong, ulong);
uvlong		memmapsize(uvlong, uvlong);
void		memmapadd(uvlong, uvlong, ulong);
uvlong		memmapalloc(uvlong, uvlong, uvlong, ulong);
void		memmapfree(uvlong, uvlong, ulong);
ulong		mcountseg(Segment*);
void		mfreeseg(Segment*, uintptr, ulong);
void		microdelay(int);
uvlong		mk64fract(uvlong, uvlong);
void		mkqid(Qid*, vlong, ulong, int);
void		mmurelease(Proc*);
void		mmuswitch(Proc*);
Chan*		mntattach(Chan*, Chan*, char*, int);
Chan*		mntauth(Chan*, char*);
int		mntversion(Chan*, char*, int, int);
void		mouseresize(void);
void		mountfree(Mount*);
ulong		ms2tk(ulong);
ulong		msize(void*);
ulong		ms2tk(ulong);
uvlong		ms2fastticks(ulong);
void		mul64fract(uvlong*, uvlong, uvlong);
void		muxclose(Mnt*);
Chan*		namec(char*, int, int, ulong);
void		namelenerror(char*, int, char*);
int		needpages(void*);
Chan*		newchan(void);
int		newfd(Chan*, int);
Mhead*		newmhead(Chan*);
Mount*		newmount(Chan*, int, char*);
Page*		newpage(int, Segment **, uintptr);
Path*		newpath(char*);
Pgrp*		newpgrp(void);
Rgrp*		newrgrp(void);
Proc*		newproc(void);
void		nexterror(void);
int		notify(Ureg*);
ulong		nkpages(Confmem*);
uvlong		ns2fastticks(uvlong);
int		okaddr(uintptr, ulong, int);
int		openmode(ulong);
Block*		packblock(Block*);
Block*		padblock(Block*, int);
void		pageinit(void);
ulong		pagereclaim(Image*);
void		panic(char*, ...);
Cmdbuf*		parsecmd(char *a, int n);
void		pathclose(Path*);
ulong		perfticks(void);
void		pexit(char*, int);
void		pgrpcpy(Pgrp*, Pgrp*);
ulong		pidalloc(Proc*);
#define		waserror()		setlabel(&up->errlab[up->nerrlab++])
#define		poperror()		up->nerrlab--
void		portcountpagerefs(ulong*, int);
char*		popnote(Ureg*);
int		postnote(Proc*, int, char*, int);
void		postnotepg(ulong, char*, int);
int		pprint(char*, ...);
void		preempted(int);
void		prflush(void);
void		printinit(void);
ulong		procalarm(ulong);
void		procctl(void);
int		procfdprint(Chan*, int, char*, int);
void		procflushseg(Segment*);
void		procflushpseg(Physseg*);
void		procflushothers(void);
int		procindex(ulong);
void		procinit0(void);
void		procinterrupt(Proc*);
ulong		procpagecount(Proc*);
void		procpriority(Proc*, int, int);
void		procsetuser(char*);
Proc*		proctab(int);
extern void	(*proctrace)(Proc*, int, vlong); 
void		procwired(Proc*, int);
Pte*		ptealloc(void);
Pte*		ptecpy(Pte*);
int		pullblock(Block**, int);
Block*		pullupblock(Block*, int);
Block*		pullupqueue(Queue*, int);
int		pushnote(Proc*, Note*);
void		putimage(Image*);
void		putmhead(Mhead*);
void		putmmu(uintptr, uintptr, Page*);
void		putpage(Page*);
void		putseg(Segment*);
void		putstrn(char*, int);
void		putswap(Page*);
ulong		pwait(Waitmsg*);
int		qaddlist(Queue*, Block*);
Block*		qbread(Queue*, int);
long		qbwrite(Queue*, Block*);
Queue*		qbypass(void (*)(void*, Block*), void*);
int		qcanread(Queue*);
void		qclose(Queue*);
int		qconsume(Queue*, void*, int);
Block*		qcopy(Queue*, int, ulong);
int		qdiscard(Queue*, int);
void		qflush(Queue*);
void		qfree(Queue*);
int		qfull(Queue*);
Block*		qget(Queue*);
void		qhangup(Queue*, char*);
int		qisclosed(Queue*);
int		qiwrite(Queue*, void*, int);
int		qlen(Queue*);
void		qlock(QLock*);
Queue*		qopen(int, int, void (*)(void*), void*);
int		qpass(Queue*, Block*);
int		qpassnolim(Queue*, Block*);
int		qproduce(Queue*, void*, int);
void		qputback(Queue*, Block*);
long		qread(Queue*, void*, int);
Block*		qremove(Queue*);
void		qreopen(Queue*);
void		qsetlimit(Queue*, int);
void		qunlock(QLock*);
int		qwindow(Queue*);
int		qwrite(Queue*, void*, int);
void		qnoblock(Queue*, int);
void		randominit(void);
ulong		randomread(void*, ulong);
void		ramdiskinit(void);
void		rdb(void);
long		readblist(Block*, uchar*, long, ulong);
int		readnum(ulong, char*, ulong, ulong, int);
int		readstr(ulong, char*, ulong, char*);
void		ready(Proc*);
void*		realloc(void *v, ulong size);
void		rebootcmd(int, char**);
void		reboot(void*, void*, ulong);
void		relocateseg(Segment*, uintptr);
void		renameuser(char*, char*);
void		resched(char*);
void		resrcwait(char*);
int		return0(void*);
void		rlock(RWlock*);
long		rtctime(void);
void		runlock(RWlock*);
Proc*		runproc(void);
void		savefpregs(FPsave*);
void		sched(void);
void		schedinit(void);
void		(*screenputs)(char*, int);
void*		secalloc(ulong);
void		secfree(void*);
long		seconds(void);
uintptr		segattach(int, char *, uintptr, uintptr);
void		segclock(uintptr);
long		segio(Segio*, Segment*, void*, long, vlong, int);
void		segpage(Segment*, Page*);
int		setcolor(ulong, ulong, ulong, ulong);
void		setkernur(Ureg*, Proc*);
int		setlabel(Label*);
void		setmalloctag(void*, uintptr);
ulong		setnoteid(Proc*, ulong);
void		setrealloctag(void*, uintptr);
void		setregisters(Ureg*, char*, char*, int);
void		setupwatchpts(Proc*, Watchpt*, int);
char*		skipslash(char*);
void		sleep(Rendez*, int(*)(void*), void*);
void*		smalloc(ulong);
int		splhi(void);
int		spllo(void);
void		splx(int);
void		splxpc(int);
char*		srvname(Chan*);
void		srvrenameuser(char*, char*);
void		shrrenameuser(char*, char*);
int		swapcount(uintptr);
int		swapfull(void);
void		syscallfmt(ulong syscallno, uintptr pc, va_list list);
void		sysretfmt(ulong syscallno, va_list list, uintptr ret, uvlong start, uvlong stop);
void		timeradd(Timer*);
void		timerdel(Timer*);
void		timersinit(void);
void		timerintr(Ureg*, Tval);
void		timerset(Tval);
ulong		tk2ms(ulong);
#define		TK2MS(x) ((x)*(1000/HZ))
uvlong		tod2fastticks(vlong);
vlong		todget(vlong*);
void		todsetfreq(vlong);
void		todinit(void);
void		todset(vlong, vlong, int);
Block*		trimblock(Block*, int, int);
void		tsleep(Rendez*, int (*)(void*), void*, ulong);
void		twakeup(Ureg*, Timer *);
int		uartctl(Uart*, char*);
int		uartgetc(void);
void		uartkick(void*);
void		uartmouse(char*, int (*)(Queue*, int), int);
void		uartsetmouseputc(char*, int (*)(Queue*, int));
void		uartputc(int);
void		uartputs(char*, int);
void		uartrecv(Uart*, char);
int		uartstageoutput(Uart*);
void		unbreak(Proc*);
void		uncachepage(Page*);
long		unionread(Chan*, void*, long);
void		unlock(Lock*);
uvlong		us2fastticks(uvlong);
void		userinit(void);
uintptr		userpc(void);
long		userwrite(char*, int);
void		validaddr(uintptr, ulong, int);
void		validname(char*, int);
char*		validnamedup(char*, int);
void		validstat(uchar*, int);
void*		vmemchr(void*, int, ulong);
Proc*		wakeup(Rendez*);
int		walk(Chan**, char**, int, int, int*);
void		wlock(RWlock*);
void		wunlock(RWlock*);
void*		xalloc(ulong);
void*		xallocz(ulong, int);
void		xfree(void*);
void		xhole(uintptr, uintptr);
void		xinit(void);
int		xmerge(void*, void*);
void*		xspanalloc(ulong, int, ulong);
void		xsummary(void);
void		yield(void);
void		zeropage(Page*);
void		zeroprivatepages(void);
Segment*	data2txt(Segment*);
Segment*	dupseg(Segment**, int, int);
Segment*	newseg(int, uintptr, ulong);
Segment*	seg(Proc*, uintptr, int);
Segment*	txt2data(Segment*);
void		hnputv(void*, uvlong);
void		hnputl(void*, uint);
void		hnputs(void*, ushort);
uvlong		nhgetv(void*);
uint		nhgetl(void*);
ushort		nhgets(void*);
ulong		µs(void);
long		lcycles(void);
void		devmask(Pgrp*,int,char*);
int		devallowed(Pgrp*, int);
int		canmount(Pgrp*);

#pragma varargck argpos iprint	1
#pragma	varargck argpos	panic	1
#pragma varargck argpos pprint	1
