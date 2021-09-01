/*
* File: testMssgBox.c
* Author: Caleb M. McLaren
* email: mclaren1@umbc.edu
* date: March 29th, 2021
* description: this is the testing driver for the system calls developed for 
* 	project 2, section 99, CMSC 421, Spring 2021, UMBC
*/

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h> /*user size_t*/
#include <string.h>
#include <errno.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>

#define __NR_create_mbox_421 436
long create_mbox_421_syscall(unsigned long id){
	return syscall(__NR_create_mbox_421);
}
#define __NR_remove_mbox_421 437
long remove_mbox_421_syscall(unsigned long id){
	return syscall(__NR_remove_mbox_421);
}
#define __NR_reset_mbox_421 438
long reset_mbox_421_syscall(void){
	return syscall(__NR_reset_mbox_421);
}
#define __NR_count_mbox_421 439
long count_mbox_421_syscall(void){
	return syscall(__NR_count_mbox_421);
}
#define __NR_list_mbox_421 440
long list_mbox_421_syscall(unsigned long __user *mbxes, long k){
	return syscall(__NR_list_mbox_421);
}
#define __NR_send_msg_421 441
long send_msg_421_syscall(unsigned long id, const unsigned char __user *msg, 
long n){
	return syscall(__NR_send_msg_421);
}
#define __NR_recv_msg_421 442
long recv_msg_421_syscall(unsigned long id, unsigned char __user *msg, long n){
	return syscall(__NR_recv_msg_421);
}
#define __NR_peek_msg_421 443
long peek_msg_421_syscall(unsigned long id, unsigned char __user *msg, long n){
	return syscall(__NR_peek_msg_421);
}
#define __NR_count_msg_421 444
long count_msg_421_syscall(unsigned long id){
	return syscall(__NR_count_msg_421);
}
#define __NR_len_msg_421 445
long len_msg_421_syscall(unsigned long id){
	return syscall(__NR_len_msg_421);
}
#define __NR_print_mbox_421 446
long print_mbox_421_syscall(unsigned long id){
	return syscall(__NR_print_mbox_421);
}

/*Constants*/
const int UNIT = 1;
const long ONE_LINE = 15;

/* Helper Function Declarations */
void display_menu();
void flush_string( char *);
char * get_user_input;


int main(void) {
    char *mboxId = NULL;
    char *menuChoice = NULL;
    long int temp, tip, torq, tr;
    unsigned long tap;
    bool flag = true;

    while (flag) {
        /*Start*/
        display_menu();
        menuChoice = get_user_input();
        flush_string(menuChoice);
        temp = strtol(menuChoice, NULL, 0);
        free(menuChoice);
        menuChoice = NULL;

        /*Check user choice*/
        switch (temp) {
            case 1:
                /*get user input*/
                printf("%s", "Please enter a mailbox id number: ");
                mboxId = get_user_input();
                flush_string(mboxId);
                tip = strtol(mboxId, NULL, 0);
                free(mboxId);
                mboxId = NULL;
                tap = (unsigned long) tip;

                /*create mailbox*/
                tip = create_mbox_421_syscall(tap);
                
                /*check */
                if ( tip == 0 ) printf("%s\n", "Success.");
                else if (-tip == EPERM ) printf("%s\n", "Must be root user");
                else if (-tip == ENOMEM) printf("%s\n", 
                "Bad memory allocation at create_mbox");
                else if (-tip == EEXIST) printf("%s\n", 
                "That maibox already exists.");

                continue;

            case 2:
                /*get user input*/
                printf("%s", "Enter the mailbox number for deletion: ");
                mboxId = get_user_input();
                flush_string(mboxId);
                torq = strtol(mboxId, NULL, 0);
                free(mboxId);
                mboxId = NULL;
                
                /*remove mailbox*/
                tip = remove_mbox_421_syscall(torq);
                
                /*check*/
                if (tip == 0 ) printf("%s\n", "Mbox Removed.");
                else if (-tip == EPERM) printf("%s\n", "Must be root user");
                else if(-tip == ENOTEMPTY) printf("%s\n", 
                "mbox is not empty, empty mailbox prior to deletion");
                else if (-tip == ENOENT) printf("%s\n", 
                "mbox specified does not exist");
                
                continue;

            case 3:
                /*confirm selection*/
                printf("%s\n", 
                "Confirm you want to delete all mailboxes and messages? Y/n: ");
                mboxId = get_user_input();
                flush_string(mboxId);
                if (strcmp(mboxId, "Y") || strcmp(mboxId, "y")) {
					
					/*reset mailbox stand*/
                    tip = reset_mbox_421_syscall();
                    if (tip == 0) printf("%s\n", 
                    "All mailboxes and messages were deleted.");
                    else if (-tip == EPERM) printf("%s\n","Must be root user");
                }
                free(mboxId);
                mboxId = NULL;
                
                continue;

            case 4:
                /*count mailboxes attached to the mbox_stand*/
                tip = count_mbox_421_syscall();
                printf("%s: %ld\n", "Mailbox count", tip);
                continue;

            case 5:
                /* get user input */
                printf("%s\n", 
                "How many mailboxes would you like to hear about?");
                mboxId = get_user_input();
                flush_string(mboxId);
                torq = strtol(mboxId, NULL, 0);
                free(mboxId);
                mboxId = NULL;
                unsigned long *p = calloc(torq, sizeof(unsigned long));
               
               	/*return a list of mailbox ids, and the int count */
                tr = list_mbox_421_syscall(p, torq);
                
                puts("######## Mailboxes requested ########");
                printf("%s: %ld\n", "Number of Mailboxes: ", tr);
                for (long j = 0; j < torq; j++) {
                    printf("%s: %ld\n", "Mailbox ", p[j]);
                }
                puts("######## END Mailboxes requested ########");
                continue;

            case 6:
                /* get user input*/ 
                printf("%s\n", "Which mailbox?");
                mboxId = get_user_input();
                flush_string(mboxId);
                tip = strtol(mboxId, NULL, 0);
                free(mboxId);
                mboxId = NULL;
                tap = (unsigned long) tip;
                
                /*small message*/
               	const unsigned char *d;
                unsigned char myUnsignedCharArray[6] = {'h', 'e', 'l',
                'l', 'o', '\n'}  
                d = myUnsignedCharArray;
                
                /*large message*/

                printf("%s\n", "How long should the message be: ");
                mboxId = get_user_input();
                flush_string(mboxId);
                tip = strtol(mboxId, NULL, 0);
                free(mboxId);
                mboxId = NULL;

                /*Now put message in a mailbox */
                tip = send_msg_421_syscall(tap, d, tip);
                
            	/*check */
            	if(-tip == EFAULT) printf("%s\n",
            	"Null pointer or bad access");
            	else if(-tip == ENOENT) printf("%s\n","Mailbox Not Found");
            	else if(-tip == EPERM) printf("%s\n", "n less than 0");
                else printf("%s, %ld\n", "Size of the message: ", tip);
                
                continue;

            case 7:
                /* get user input */
                printf("%s\n", 
                "Which mailbox? You will receive the first message in line.");
                mboxId = get_user_input();
                flush_string(mboxId);
                tip = strtol(mboxId, NULL, 0);
                free(mboxId);
                mboxId = NULL;
                tap = (unsigned long) tip;
              
                /* get user input */
                printf("%s\n", "How long should the message be: ");
                mboxId = get_user_input();
                flush_string(mboxId);
                tip = strtol(mboxId, NULL, 0);
                free(mboxId);
                mboxId = NULL;

                /*Set aside buffer to receive mssg in user space*/
                unsigned char *w = calloc(tip, sizeof(unsigned char));
                tip = recv_msg_421_syscall(tap, w, tip);
                if(-tip == EFAULT) printf("%s\n", 
                "Bad copy_to_user, Bad access_ok, or null pointer");
                else if(-tip == ENOENT) printf("%s\n", "Mailbox not found");
                else if(-tip == EPERM) printf("%s\n", "n less than 0");
                else printf("%s\n", w);
                free(w);
                
                continue;

            case 8:
                /*peek at a message*/
                continue;

            case 9:
                /*count the messages*/
                continue;
                
           	case 10:
           		/*Query length of a message*/
           		continue;
           		
           	case 11:
           		/*print all messages in one mailbox*/
           		continue;
           		
           	case 12: 
           		/*quit loop*/
                flag = false;
           		continue; 

            default:
                /*print default*/
                printf("%s\n", "No one is coming to save us.");
        }
    }

    return 0;
}


void display_menu(){
    printf("%s\n", "Create Mailbox ------------- 1");
    printf("%s\n", "Delete Mailbox ------------- 2");
    printf("%s\n", "Reset Mailbox Stand -------- 3");
    printf("%s\n", "Count the Mailboxes -------- 4");
    printf("%s\n", "Return a List of Mailboxes - 5");
    printf("%s\n", "Send Message --------------- 6");
    printf("%s\n", "Receive Message ------------ 7");
   	printf("%s\n", "Peek at a Message ---------- 8");
   	printf("%s\n", "Count the Messages --------- 9");
   	printf("%s\n", "Query Length of Message --- 10");
   	printf("%s\n", "Print Messages in Mailbox - 11");
    printf("%s\n", "Quit ---------------------- 12");
}

void flush_string( char * cp){
    char * temp;
    if( (temp = strchr(cp, '\n')) != NULL)
        *temp = '\0';
}

char * get_user_input() {

    char *listlessLine = NULL;
    size_t len = 0;
    /*"signed size_t" may hold the number -1 */
    ssize_t charRead;

    /*getline allocates a buffer for storing the line when I set up listlessLine as Null and len as 0.*/
    if( (charRead = getline(&listlessLine, &len, stdin)) != -(UNIT)){
        /*do nothing */
    } else {
        perror("error: get_user_command()");
        exit(EXIT_FAILURE);
    }

    /* return the buffer with input */
    return listlessLine;
}
