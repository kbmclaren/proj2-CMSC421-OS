 # proj2-CMSC421-OS
 ## Description
 Implement the system calls enabling an IPC mailbox system.<br>
 My approach relied intensively on the development of a detailied prototype.After figuring out and testing mostly in the protoype, I transfered and translated my system calls to the kernel. I developed a test driver(incomplete), compiled the kernel, installed the newly compiled kernel, polished up the Readme files, and removed extraneous Clion files.<br>
 
 ## Repo-Contents
 - ...<br>
 - proj2:<br>
    - MAKEFILE<br>
    - README: This README must detail the information necessary to understand the associated project 2. <br>
    - mssgBox.c: This file must implement the system calls enabling an IPC mailbox system. Original content implemented in kernel space.<br>
 - proj2proto:<br>
    - src :<br>
        - proto.c : userspace driver, used to test system call algorithm in user space outside of kernal space.<br>
    - userSpaceList : <br>
        - list.h : list.h, based on linux kernel v5.5.0 list.h Modified by Ioannis Boutsikas to work in userspace<br>
        - list_hacks.h : list_hacks.h -- Modifications to make list.h usuable in userspace. Copyright (C) 2018, 2021 Ioannis Boutsikas <br>
    - CMakeList.txt
    - Makefile
    - README: This document must detail the information necessary to understand the associated project 2 prototype.<br>
 - proj2test:<br>
    - Makefile<br>
    - README.md: this README must detail the testing and verification regimine of the system calls implemented for  project 2, Section 99, CMSC 421 OS, UMBC, in the Spring Semester of 2021.<br>
    - testMssgBox.c: this is the testing driver for the system calls developed for project 2, section 99, CMSC 421, Spring 2021, UMBC<br>
 - ...<br>
 - README: The current document.<br>
 - README-LINUX : Readme provided by the project architect.<br>
 - README-SUBMISSION : Overall Readme submitted for project grading. Details of my approach to developemnt, references, and additional information for the grader.<br>

