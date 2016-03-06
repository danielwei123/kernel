Documentation for Kernel Assignment 1
=====================================

+-------------+
| BUILD & RUN |
+-------------+

Comments: 

steps:
1.	make
2.	./weenix -n
3.	make clean will delete all the executables
4.	Additional tests under section (E) have been implemented as commands in the kshell

+-----------------+
| SKIP (Optional) |
+-----------------+

Is there are any tests in the standard test suite that you know that it's not
working and you don't want the grader to run it at all so you won't get extra
deductions, please list them here.  (Of course, if the grader won't run these
tests, you will not get plus points for them.)

+---------+
| GRADING |
+---------+

(A.1) In main/kmain.c:
    (a) In bootstrap(): 3 out of 3 pts
    (b) In initproc_create(): 3 out of 3 pts

(A.2) In proc/proc.c:
    (a) In proc_create(): 4 out of 4 pts
    (b) In proc_cleanup(): 5 out of 5 pts
    (c) In do_waitpid(): 8 out of 8 pts

(A.3) In proc/kthread.c:
    (a) In kthread_create(): 2 out of 2 pts
    (b) In kthread_cancel(): 1 out of 1 pt
    (c) In kthread_exit(): 3 out of 3 pts

(A.4) In proc/sched.c:
    (a) In sched_wakeup_on(): 1 out of 1 pt
    (b) In sched_make_runnable(): 1 out of 1 pt

(A.5) In proc/kmutex.c:
    (a) In kmutex_lock(): 1 out of 1 pt
    (b) In kmutex_lock_cancellable(): 1 out of 1 pt
    (c) In kmutex_unlock(): 1 out of 2 pts

(B) Kshell : 20 out of 20 pts
    Comments: kshell works as intended. 'help' shows the commands that are available.

(C.1) waitpid any test, etc. (4 out of 4 pts)
(C.2) Context switch test (1 out of 1 pt)
(C.3) wake me test, etc. (2 out of 2 pts)
(C.4) wake me uncancellable test, etc. (2 out of 2 pts)
(C.5) cancel me test, etc. (4 out of 4 pts)
(C.6) reparenting test, etc. (2 out of 2 pts)
(C.7) show race test, etc. (3 out of 3 pts)
(C.8) kill child procs test (2 out of 2 pts)
(C.9) proc kill all test (2 out of 2 pts)

(D.1) sunghan_test(): producer/consumer test (9 out of 9 pts)
(D.2) sunghan_deadlock_test(): deadlock test (4 out of 4 pts)

(E) Additional self-checks: (10 out of 10 pts)
    Comments: (please provide details, add subsections and/or items as needed)

(E.1) Big Process Name(more than 256 characters)
(E.2) Thread self cancel
(E.3) Init clean proc same as calling exit on the kshell
(E.4) Bad arguments- pid number other than -1 and positive numbers
(E.5) Bad arguments- options other than zero

Missing required section(s) in README file (procs-README.txt): -
Submitted binary file : -
Submitted extra (unmodified) file : -
Wrong file location in submission : -
Altered or removed top comment block in a .c file : -
Use dbg_print(...) instead of dbg(DBG_PRINT, ...) : used dbg(DGB_PRINT, ..)
Not properly indentify which dbg() printout is for which item in the grading guidelines : -
Cannot compile : -
Compiler warnings : none
"make clean" : deleted all the executables
Useless KASSERT : -
Insufficient/Confusing dbg : -
Kernel panic : -
Cannot halt kernel cleanly : Kernel halts cleanly after user exits from kshell.

+------+
| BUGS |
+------+

Comments: none

+---------------------------+
| CONTRIBUTION FROM MEMBERS |
+---------------------------+

If not equal-share contribution, please list percentages.

+------------------+
| OTHER (Optional) |
+------------------+

Special DBG setting in Config.mk for certain tests: DRIVERS = 1, DBG = all
Comments on deviation from spec (you will still lose points, but it's better to let the grader know): -
General comments on design decisions: DRIVERS flag to be set to 1.

