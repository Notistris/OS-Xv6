Student: Minh-Huy Mai Duc<br>
StudentID: 23122008<br>
Institution: VNUHCM University of Science

To add a new syscall to the current xv6 setup; you need to modify at least these files:

Kernel space:<br>
+-- [syscall.h](kernel/syscall.h): define a new system call number (usually increased by 1 from the last one).<br>
|<br>
+-- [syscall.c](kernel/syscall.c): define the prototype for the function that handles the system call and map the previous defined system call number to that function. <br>
|<br>
+-- [sysproc.c](kernel/sysproc.c): implement for the system call function. <br>
|<br>
+-- dependencies files<br>
<br>
User space:<br>
+-- [user.h](user/user.h): define the function that the user can use to invoke the new implemented system call. <br>
|<br>
+-- [usys.pl](user/usys.pl): add an interface for the user function to access the system call. (e.g entry('newfunc'))<br>
|<br>
+-- mainprogram.c - e.g: [helloworld.c](user/testhello.c) or [trace.c](user/trace.c): typically handle logic at input level, invoking the function implemented from [...]<br>
|<br>
+-- dependencies files<br>
