Documentation for Kernel Assignment 2
=====================================

+-------------+
| BUILD & RUN |
+-------------+

Comments: ?

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
    (a) In special_file_read(): ? out of 6 pts
    (b) In special_file_write(): ? out of 6 pts

(A.2) In fs/namev.c:
    (a) In lookup(): ? out of 6 pts
    (b) In dir_namev(): ? out of 10 pts
    (c) In open_namev(): ? out of 2 pts

(A.3) In fs/vfs_syscall.c:
    (a) In do_write(): ? out of 6 pts
    (b) In do_mknod(): ? out of 2 pts
    (c) In do_mkdir(): ? out of 2 pts
    (d) In do_rmdir(): ? out of 2 pts
    (e) In do_unlink(): ? out of 2 pts
    (f) In do_stat(): ? out of 2 pts

(B) vfstest: ? out of 39 pts
    Comments: ?

(C.1) faber_fs_thread_test (? out of 3 pts)
(C.2) faber_directory_test (? out of 2 pts)

(D) Self-checks: (? out of 10 pts)
    Comments: ? (please provide details, add subsections and/or items as needed;
                 or, say that none is needed)

Missing required section(s) in README file (vfs-README.txt): (Comments?)
Submitted binary file : (Comments?)
Submitted extra (unmodified) file : (Comments?)
Wrong file location in submission : (Comments?)
Altered or removed top comment block in a .c file : (Comments?)
Use dbg_print(...) instead of dbg(DBG_PRINT, ...) : (Comments?)
Not properly indentify which dbg() printout is for which item in the grading guidelines : (Comments?)
Cannot compile : (Comments?)
Compiler warnings : (Comments?)
"make clean" : (Comments?)
Useless KASSERT : (Comments?)
Insufficient/Confusing dbg : (Comments?)
Kernel panic : (Comments?)
Cannot halt kernel cleanly : (Comments?)

+------+
| BUGS |
+------+

Comments: ?

+---------------------------+
| CONTRIBUTION FROM MEMBERS |
+---------------------------+

If not equal-share contribution, please list percentages.

+------------------+
| OTHER (Optional) |
+------------------+

Special DBG setting in Config.mk for certain tests: ?
Comments on deviation from spec (you will still lose points, but it's better to let the grader know): ?
General comments on design decisions: ?

