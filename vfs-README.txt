Documentation for Kernel Assignment 2
=====================================

+-------------+
| BUILD & RUN |
+-------------+

Comments:

steps:
1.	make
2.	./weenix -n
3.	make clean will delete all the executables

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

(A.1) In fs/vnode.c:
    (a) In special_file_read(): 6 out of 6 pts
    (b) In special_file_write(): 6 out of 6 pts

(A.2) In fs/namev.c:
    (a) In lookup(): 6 out of 6 pts
    (b) In dir_namev(): 10 out of 10 pts
    (c) In open_namev(): 2 out of 2 pts

(A.3) In fs/vfs_syscall.c:
    (a) In do_write(): 6 out of 6 pts
    (b) In do_mknod(): 2 out of 2 pts
    (c) In do_mkdir(): 2 out of 2 pts
    (d) In do_rmdir(): 2 out of 2 pts
    (e) In do_unlink(): 2 out of 2 pts
    (f) In do_stat(): 2 out of 2 pts

(B) vfstest: 39 out of 39 pts
    Comments: Kernel halts cleanly on exit after running vfs test twice.

(C.1) faber_fs_thread_test (3 out of 3 pts)
(C.2) faber_directory_test (2 out of 2 pts)

(D) Self-checks: (10 out of 10 pts)
    Comments: ? (please provide details, add subsections and/or items as needed;
                 or, say that none is needed)

Missing required section(s) in README file (vfs-README.txt): -
Submitted binary file : -
Submitted extra (unmodified) file : -
Wrong file location in submission : -
Altered or removed top comment block in a .c file : -
Use dbg_print(...) instead of dbg(DBG_PRINT, ...) : Used dbg(DBG_PRINT, ...)
Not properly indentify which dbg() printout is for which item in the grading guidelines : -
Cannot compile : -
Compiler warnings : None
"make clean" : Deleted all the executables
Useless KASSERT : None
Insufficient/Confusing dbg : -
Kernel panic : -
Cannot halt kernel cleanly : Kernel halts cleanly after user exits from kshell.

+------+
| BUGS |
+------+

Comments: None

+---------------------------+
| CONTRIBUTION FROM MEMBERS |
+---------------------------+

If not equal-share contribution, please list percentages.

+------------------+
| OTHER (Optional) |
+------------------+

Special DBG setting in Config.mk for certain tests: DRIVERS = 1, VFS = 1, DBG = all
Comments on deviation from spec (you will still lose points, but it's better to let the grader know): -
General comments on design decisions: DRIVERS and VFS flags are to be set to 1.
