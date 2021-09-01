/*
* File: proto.c
* Author: Caleb M. McLaren
* email: mclaren1@umbc.edu
* date: March 8th, 2021
* description:perhaps this should be the driver, calling the functions necessary for the mailbox
*/


#include <stdio.h>
#include <stdlib.h>
#include <stddef.h> /*user size_t*/
#include "../userSpaceList/list.h"
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>



typedef struct message {
    long len;
    unsigned char * the_mssg_itself;
    struct list_head i_link_messages;
} message_t;

typedef struct mailbox {
    unsigned long id;
    struct list_head mssg_anchor;
    struct list_head i_link_mbox;

} mailbox_t;

const int UNIT = 1;
const long ONE_LINE = 15;
const int PAIR = 2;

/* Declarations */
long print_mbox(unsigned long id);
long len_msg(unsigned long id);
long count_msg(unsigned long id);
long peek_msg(unsigned long id, unsigned char *msg, long n);
long recv_msg(unsigned long, unsigned char *, long);
long send_mssg(unsigned long, const unsigned char *, long );
bool check_for_mbox(unsigned long);
long list_mbox(unsigned long *, long);
long count_mbox();
long reset_mbox_stand();
long remove_mbox(unsigned long);
void print_mbox_list();
long create_mbox(unsigned long);
void display_menu();
void flush_string( char *);
char * get_user_input();
LIST_HEAD(mbox_stand);


int main(void) {
    char *mboxId = NULL;
    char *menuChoice;
    long int temp, tip;
    long int torq, tr;
    unsigned long tap;
    bool flag = true;
    /*long int;*/

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
                tip = create_mbox(tap);
                if (tip == ENOMEM) {
                    printf("%s\n", "Bad memory allocation at create_mbox");
                } else if (tip == EEXIST) {
                    printf("%s\n", "That maibox already exists.");
                }
                continue;

            case 2:
                /*delete a mailbox*/
                print_mbox_list();
                /*get user input*/
                printf("%s", "Enter the mailbox number for deletion: ");
                mboxId = get_user_input();
                flush_string(mboxId);
                torq = strtol(mboxId, NULL, 0);
                free(mboxId);
                mboxId = NULL;
                tip = remove_mbox(torq);
                if (tip == ENOTEMPTY) {
                    printf("%s\n", "mbox is not empty, empty mailbox prior to deletion");
                } else if (tip == ENOENT) {
                    printf("%s\n", "mbox specified does not exist");
                } else {
                    printf("%s\n", "mbox successfully removed");
                }
                continue;

            case 3:
                /*reset mbox_stand*/
                printf("%s\n", "Are your sure you want to delete all mailboxes and messages? Y/n: ");
                mboxId = get_user_input();
                flush_string(mboxId);
                if (strcmp(mboxId, "Y") || strcmp(mboxId, "y")) {

                    tip = reset_mbox_stand();
                    if (tip == 0) {
                        printf("%s\n", "All mailboxes and messages were deleted.");
                        print_mbox_list();
                    } else if (tip == UNIT) {
                        printf("%s\n", "An error occured .");
                        print_mbox_list();
                    }
                }
                free(mboxId);
                mboxId = NULL;
                continue;

            case 4:
                /*count the number of mailboxes attached to the mbox_stand*/
                tip = count_mbox();
                printf("%s: %ld\n", "Mailbox count", tip);
                continue;

            case 5:
                /*return a list of mailbox ids, and the int count */
                printf("%s\n", "How many mailboxes would you like to hear about? ");
                mboxId = get_user_input();
                flush_string(mboxId);
                torq = strtol(mboxId, NULL, 0);
                free(mboxId);
                mboxId = NULL;
                unsigned long *p = calloc(torq, sizeof(unsigned long));
                for (long i = 0; i < torq; i++) {
                    p[i] = 0;
                }
                tr = list_mbox(p, torq);
                puts("######## Mailboxes requested ########");
                printf("%s: %ld\n", "Number of Mailboxes: ", tr);
                for (long j = 0; j < torq; j++) {
                    printf("%s: %ld\n", "Mailbox ", p[j]);
                }
                puts("######## END Mailboxes requested ########");
                continue;

            case 6:
                /* send message to mailbox that exists*/
                printf("%s\n", "Which mailbox?");
                print_mbox_list();
                mboxId = get_user_input();
                flush_string(mboxId);
                tip = strtol(mboxId, NULL, 0);
                free(mboxId);
                mboxId = NULL;
                tap = (unsigned long) tip;
                if (check_for_mbox(tap)) {
                    const unsigned char *d;
                    unsigned char myUnsignedCharArray[6] = {'h', 'e', 'l', 'l', 'o', '\0'};
                    d = myUnsignedCharArray;

                    printf("%s\n", "How long should the message be: ");
                    mboxId = get_user_input();
                    flush_string(mboxId);
                    tip = strtol(mboxId, NULL, 0);
                    free(mboxId);
                    mboxId = NULL;
                    long xee;
                    /*Now put message in a mailbox */
                    xee = send_mssg(tap, d, tip);

                    if (xee == ENOENT) {
                        printf("%s\n", "the mailbox was not found");
                    }
                    else if (xee == EPERM){
                        printf("%s\n", "permission denied");
                    }
                    else{
                        printf("%s:%ld\n", "Bytes copied", xee);

                    }

                } else printf("%s\n", "That mailbox does not exist.");
                continue;

            case 7:
                /* receive message from mailbox*/
                printf("%s\n", "Which mailbox? You will receive the first message in line.");
                print_mbox_list();
                mboxId = get_user_input();
                flush_string(mboxId);
                tip = strtol(mboxId, NULL, 0);
                free(mboxId);
                mboxId = NULL;
                tap = (unsigned long) tip;
                if (check_for_mbox(tap)) {
                    /*Mailbox exists*/
                    printf("%s\n", "How long should the message be: ");
                    mboxId = get_user_input();
                    flush_string(mboxId);
                    tip = strtol(mboxId, NULL, 0);
                    free(mboxId);
                    mboxId = NULL;

                    /*Set aside buffer to receive mssg in user space*/
                    unsigned char *w = calloc(tip, sizeof(unsigned char));
                    tr = recv_msg(tap, w, tip);
                    if(tr == ENOENT) {
                        printf("%s\n", "no message found");
                    } else if (tr == UNIT) {
                        printf("%s\n", "no mailbox found???");
                    } else {
                        printf("%s\n", w);
                    }
                    free(w);
                }
                continue;

            case 8:
                /*quit loop*/
                flag = false;
                continue;

            case 9:
                print_mbox_list(mbox_stand);
                continue;

            default:
                /*print default*/
                printf("%s\n", "No one is coming to save us.");
        }
    }

    return 0;
}

long print_mbox(unsigned long id){
    /*find mailbox */
    mailbox_t *the_mailbox;
    message_t *the_mssg;
    long int i ;


    list_for_each_entry(the_mailbox, &mbox_stand, i_link_mbox) {
        /*when you find the mailbox, receive the message*/
        if (id == the_mailbox->id) {
            /*Mailbox not empty*/
            list_for_each_entry(the_mssg, &the_mailbox->mssg_anchor, i_link_messages) {
                for(i = 0; i < the_mssg->len; i++){
                    printf("%02x ", the_mssg->the_mssg_itself[i]);
                    if( i%ONE_LINE == 0 ){
                        printf("\n");
                    }
                }
                printf("%s\n", "---");
            }
            /*when there are no messages*/
            return -EFAULT;
        }
    }
    return -ENOENT;
}

long len_msg(unsigned long id) {
    /*find mailbox */
    mailbox_t *the_mailbox;
    message_t *the_mssg;

    list_for_each_entry(the_mailbox, &mbox_stand, i_link_mbox) {
        /*when you find the mailbox, receive the message*/
        if (id == the_mailbox->id) {
            /*Mailbox not empty*/
            list_for_each_entry(the_mssg, &the_mailbox->mssg_anchor, i_link_messages) {
                return the_mssg->len;
            }
            /*when there are no messages*/
            return -EFAULT;
        }
    }
    return -ENOENT;
}

long count_msg(unsigned long id){
    /*find mailbox */
    mailbox_t * the_mailbox;
    message_t *the_mssg;
    long int temp = 0;
    list_for_each_entry(the_mailbox, &mbox_stand, i_link_mbox) {
        /*when you find the mailbox, receive the message*/
        if (id == the_mailbox->id) {

            list_for_each_entry(the_mssg, &the_mailbox->mssg_anchor, i_link_messages) {
                /*Mailbox not empty*/
                temp++;
            }
            return temp;
        }
    }
    return -ENOENT;
}

long peek_msg(unsigned long id, unsigned char *msg, long n){
    if(msg == NULL){
        return EFAULT;
    }
    /*access_ok here?*/
    if( n < 0){
        return EPERM;
    }
    /*find mailbox */
    mailbox_t * the_mailbox;
    list_for_each_entry(the_mailbox, &mbox_stand, i_link_mbox) {
        /*when you find the mailbox, receive the message*/
        if (id == the_mailbox->id) {
            /*Mailbox not empty*/
            message_t *the_mssg;

            list_for_each_entry(the_mssg, &the_mailbox->mssg_anchor, i_link_messages) {
                memmove(msg, the_mssg->the_mssg_itself, n);
                return n;
            }
            return -EFAULT;
        }
    }
    return -ENOENT;
}

long recv_msg(unsigned long id, unsigned char *msg, long n){
    if(msg == NULL){
        return EFAULT;
    }
    /*access_ok here?*/
    if( n < 0){
        return EPERM;
    }
    /*find mailbox */
    mailbox_t * the_mailbox;
    list_for_each_entry(the_mailbox, &mbox_stand, i_link_mbox) {
        /*when you find the mailbox, receive the message*/
        if (id == the_mailbox->id) {
            /*Mailbox not empty*/
            message_t *the_mssg;
            message_t *temp_mssg;

            list_for_each_entry_safe(the_mssg, temp_mssg, &the_mailbox->mssg_anchor, i_link_messages) {
                //memcpy(msg, the_mssg->the_mssg_itself, n);
                memmove(msg, the_mssg->the_mssg_itself, n);
                /*Remove the mssg from the mailbox*/
                list_del(&the_mssg->i_link_messages);
                free(the_mssg);
                /*interupt the for loop*/
                return n;
            }
            /**/
            return -EFAULT;
        }
    }
    /*mailbox not found*/
    return ENOENT;
}

/*Move to kernel where you can use access_ok*/
long send_mssg(unsigned long id, const unsigned char * msg, long n){
    if(msg == NULL){
        return EFAULT;
    }
    /*access_ok here?*/
    if( n < 0){
        return EPERM;
    }

    mailbox_t * the_mailbox;
    mailbox_t * temp_mailbox;
    list_for_each_entry_safe(the_mailbox, temp_mailbox, &mbox_stand, i_link_mbox) {
        /*when you find the mailbox, insert the message*/
        if (id == the_mailbox->id) {

           /* when n == 0, the malloc returns NULL which may still be freed. */
            message_t *new_mssg = (message_t *) malloc(sizeof(message_t));
            new_mssg->len = n;
            new_mssg->the_mssg_itself = malloc(n);
            memmove(new_mssg->the_mssg_itself, msg, n);
            list_add_tail(&new_mssg->i_link_messages, &the_mailbox->mssg_anchor);
            return n;
        }
    }
    /*mailbox not found*/
    return ENOENT;
}

bool check_for_mbox(unsigned long id){
    mailbox_t* p = NULL;
    list_for_each_entry(p, &mbox_stand, i_link_mbox) {
        if( id == p->id){
            return true;
        }
    }
    return false;
}

/*takes in a pointer to an empty array, returns count of how many mailbox address were added*/
long list_mbox( unsigned long * arrayPtr, long thisMany){
    long temp = 0;
    mailbox_t* p = NULL;
    list_for_each_entry(p, &mbox_stand, i_link_mbox) {
        if (temp == thisMany){
            return temp;
        }
        arrayPtr[temp] = p->id;
        temp++;
    }

    return temp;
}

/*count the number of mailboxes attached to the mbox_stand*/
long count_mbox(){
    mailbox_t* p = NULL;
    long int temp = 0;
    list_for_each_entry(p, &mbox_stand, i_link_mbox) {
        temp++;
    }
    return temp;
}

/*NOT finished*/
/*delete all messages and remove all mboxs*/
long reset_mbox_stand(){
    mailbox_t * the_mailbox;
    mailbox_t * temp_mailbox;
    list_for_each_entry_safe(the_mailbox, temp_mailbox, &mbox_stand, i_link_mbox){
        /*loop over mbox messages*/
        message_t * the_mssg;
        message_t * temp_mssg;

        list_for_each_entry_safe(the_mssg, temp_mssg, &the_mailbox->mssg_anchor, i_link_messages){
            /*Remove the mssg from the mailbox*/
            list_del(&the_mssg->i_link_messages);
            /* free the allocated memory*/
            /*this will get more complicated?*/
            free(the_mssg);
        }

        /*Remove the mailbox*/
        list_del(&the_mailbox->i_link_mbox);
        free(the_mailbox);
    }
    return 0;
}

long remove_mbox(unsigned long mboxId){
    struct list_head* pos = NULL;
    struct list_head* tmp;
    list_for_each_safe(pos, tmp, &mbox_stand){
        mailbox_t * q = list_entry(pos, mailbox_t, i_link_mbox);

        /*check if mailbox already exists*/
        if(mboxId == q->id) {
            printf("That mailbox already exists.\n");

            /*check if messages inside*/
            if(list_empty_careful(&q->mssg_anchor))
            {
                /*mbox is empty, remove mbox*/
                list_del(pos);
                free(q);
                return 0;
            }
            /*error mailbox not empty*/
            return ENOTEMPTY;
        }
    }
    /*mail box was not found*/
    printf("That mailbox does not exist.\n");
    return ENOENT;
}

void print_mbox_list(){
    /*print out a list of mailboxes*/
    puts("########## mailbox list ##########");
    printf("mbox_stand : {%p, %p} | %p\n", mbox_stand.prev, mbox_stand.next, &mbox_stand);
    mailbox_t* p = NULL;
    list_for_each_entry(p, &mbox_stand, i_link_mbox) {
        printf("Mailbox %ld {%p, %p} | %p}\n", p->id, p->i_link_mbox.prev, p->i_link_mbox.next, &p->i_link_mbox.next);
    }
    puts("########## mailbox list end ##########");
}

long int create_mbox(unsigned long id){
    /*check if mailbox already exists*/
    mailbox_t* p = NULL;
    list_for_each_entry(p, &mbox_stand, i_link_mbox){
        if( id == p->id) {
            //printf("That mailbox already exists.\n");
            return EEXIST;
        }
    }

    /*allocate a new mailbox*/
    mailbox_t *new_mbox = (mailbox_t *)malloc(sizeof(mailbox_t));
    new_mbox->id = id;
    INIT_LIST_HEAD(&new_mbox->mssg_anchor);
    /* add the new mailbox to the mbox_stand,*/
    list_add_tail(&new_mbox->i_link_mbox, &mbox_stand);

    puts("===== list as it grows =====");
    printf("mbox_stand : {%p, %p} | %p\n", mbox_stand.prev, mbox_stand.next, &mbox_stand);
    struct list_head *pos = NULL;
    list_for_each(pos, &mbox_stand) {
        p = list_entry(pos, mailbox_t, i_link_mbox);
        printf("Mailbox %lu {%p, %p} | %p}\n", p->id, p->i_link_mbox.prev, p->i_link_mbox.next, &p->i_link_mbox.next);
    }
    puts("===== list end =====");

    /*if malloc was bad then new_box will be NULL*/
    return new_mbox == NULL ? ENOMEM : 0; /*new_mbox should be a NULL pointer if MALLOC FAILED */
}

void display_menu(){
    printf("%s\n", "Create mailbox - 1");
    printf("%s\n", "Delete mailbox - 2");
    printf("%s\n", "Reset mailbox stand - 3");
    printf("%s\n", "Count the mailboxes - 4");
    printf("%s\n", "Return a list of mailboxes - 5");
    printf("%s\n", "Send Message - 6");
    printf("%s\n", "Receive Message - 7");
    printf("%s\n", "Quit - 8");
    printf("%s\n", "Print mailbox list - 9");
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