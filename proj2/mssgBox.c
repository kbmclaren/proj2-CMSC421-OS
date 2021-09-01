/*
* File: mssgBox.c
* Author: Caleb M. McLaren
* Email: mclaren1@umbc.edu
* Date: March 28, 2021
* Description: This file must implement the system calls enabling an IPC mailbox
* 			system. 
*/

#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/uidgid.h>
#include <linux/cred.h>
#include <linux/rwsem.h>

typedef struct message {
	unsigned long len;
	unsigned char * the_mssg_itself;
	struct list_head i_link_messages;
} message_t;

typedef struct mailbox {
	unsigned long id; 
	struct list_head mssg_anchor;
	struct list_head i_link_mbox; 
} mailbox_t;
 
LIST_HEAD(mbox_stand);
const long ONE_LINE = 15; 
static DECLARE_RWSEM(mbox_stand_semaphore);


SYSCALL_DEFINE1(create_mbox_421, unsigned long, id)
{
	mailbox_t *p = NULL;
	mailbox_t *new_mbox = NULL;
	kuid_t rootUid;
	rootUid.val = 0; 
	
	if(!uid_eq(get_current_cred()->uid, rootUid)){
		return -EPERM; 
	} 
	 
	list_for_each_entry(p, &mbox_stand, i_link_mbox){
		if( id == p->id){
			return -EEXIST;
		}
	}
	new_mbox = (mailbox_t *)kmalloc(sizeof(mailbox_t), GFP_KERNEL);
	if (new_mbox == NULL){
		return -ENOMEM;
		}
	new_mbox->id = id;
	INIT_LIST_HEAD(&new_mbox->mssg_anchor);
	
	/*lock. Acquire mbox_stand_semaphore as a writer to add a new mailbox*/
	down_write(&mbox_stand_semaphore);
	list_add_tail(&new_mbox->i_link_mbox, &mbox_stand);
	/*unlock*/
	up_write(&mbox_stand_semaphore);
	
	return 0; 
}

SYSCALL_DEFINE1(remove_mbox_421, unsigned long, id){
	struct list_head *pos = NULL; 
	struct list_head *tmp = NULL;
	mailbox_t *q = NULL;
	kuid_t rootUid;
	rootUid.val = 0; 

	if(!uid_eq(get_current_cred()->uid, rootUid)){
		return -EPERM; 
	} 
	/*lock mbox_stand as writer. then check if mailbox exist, 
	* to prevent dissapearance or movement in memory of mailbox by 
	* another process after checking*/
	down_write(&mbox_stand_semaphore);
	list_for_each_safe(pos, tmp, &mbox_stand){
	
		q = list_entry(pos, mailbox_t, i_link_mbox);
		if(id == q->id){
			
			if(list_empty_careful(&q->mssg_anchor)){
				
				list_del(pos);
				kfree(q);
				/*unlock, mbox successfully removed*/
				up_write(&mbox_stand_semaphore);
				return 0;
			}
			/*unlock, mailbox not empty*/
			up_write(&mbox_stand_semaphore);
			return -ENOTEMPTY;
		}
	}
	/*unlock, target mailbox not found*/
	up_write(&mbox_stand_semaphore);
	return -ENOENT;
}

SYSCALL_DEFINE0(reset_mbox_421){
	mailbox_t *the_mailbox;
	mailbox_t *temp_mailbox;
	message_t *the_mssg;
	message_t *temp_mssg;
	kuid_t rootUid;
	rootUid.val = 0; 

	if(!uid_eq(get_current_cred()->uid, rootUid)){
		return -EPERM; 
	} 

	/*lock mbox_stand as writer.*/
	down_write(&mbox_stand_semaphore);
	list_for_each_entry_safe(the_mailbox, temp_mailbox, &mbox_stand, 
								i_link_mbox){
		 
		list_for_each_entry_safe(the_mssg, temp_mssg, &the_mailbox->mssg_anchor,
									i_link_messages){
			list_del(&the_mssg->i_link_messages);
			kfree(the_mssg);
		}
		list_del(&the_mailbox->i_link_mbox);
		kfree(the_mailbox); 
	}
	/*unlock, mbox_stand successfully reset*/
	up_write(&mbox_stand_semaphore);
	return 0; 
}

SYSCALL_DEFINE0(count_mbox_421){
	mailbox_t *p = NULL;
	long int temp = 0; 
	
	/*lock mbox_stand as reader, don't want the count to change midcount*/
	down_read(&mbox_stand_semaphore);
	list_for_each_entry(p, &mbox_stand, i_link_mbox){
		temp++;
	}
	/*unlock, mailboxes successfully counted*/
	up_read(&mbox_stand_semaphore);
	return temp;
}

SYSCALL_DEFINE2(list_mbox_421, unsigned long __user *, mbxes, long, k){
	long int temp = 0; 
	mailbox_t *p = NULL;
	
	/*lock mbox_stand as reader, don't want list growing/shrinking mid tally*/
	down_read(&mbox_stand_semaphore);
	list_for_each_entry(p, &mbox_stand, i_link_mbox){
		if(temp == k){
			return temp;
		}
		mbxes[temp] = p->id;
		temp++;
	}
	/*unlock*/
	up_read(&mbox_stand_semaphore);
	return temp; 
}

SYSCALL_DEFINE3(send_msg_421, unsigned long, id, const unsigned char __user *, 
msg, long, n){
	mailbox_t *the_mailbox = NULL;
	mailbox_t *temp_mailbox = NULL;
	message_t *new_mssg = NULL;

	if(msg == NULL){
		return -EFAULT;
	}
	if( !access_ok(msg, n)){
		return -EFAULT;
	}
	if(n < 0 ){
		return -EPERM;
	}
	/*lock as writer, don't want my mailbox dissapearing before I add my 
	* message*/
	down_write(&mbox_stand_semaphore);
	list_for_each_entry_safe(the_mailbox, temp_mailbox, &mbox_stand, 
								i_link_mbox){
		if(id == the_mailbox->id){
			
			new_mssg = (message_t *)kmalloc(sizeof(message_t),GFP_KERNEL);
			if( new_mssg == NULL ){
				/*bad memory allocation, release semaphore*/
				up_write(&mbox_stand_semaphore);
				return -ENOMEM;
			}
			
			new_mssg->len = n; 
			
			new_mssg->the_mssg_itself = kmalloc(n,GFP_KERNEL);
			if( new_mssg->the_mssg_itself == NULL ){
				kfree(new_mssg);
				new_mssg = NULL;
				/*bad memory allocation, release semaphore*/
				up_write(&mbox_stand_semaphore);
				return -ENOMEM;
			}
			
			if( copy_from_user(new_mssg->the_mssg_itself, msg, n) == 0 ){
				list_add_tail(&new_mssg->i_link_messages, 
								&the_mailbox->mssg_anchor);
				/*mssg added to mailbox, unlock*/
				up_write(&mbox_stand_semaphore);
				return n; 
			}
			/*unable to read/write all memory, manage mem, unlock*/
			kfree(new_mssg->the_mssg_itself);
			new_mssg->the_mssg_itself = NULL;
			kfree(new_mssg);
			new_mssg = NULL;
			
			up_write(&mbox_stand_semaphore);
			return -EFAULT;
		}
	}
	/*mailbox not found, unlock*/
	up_write(&mbox_stand_semaphore);
	return -ENOENT;
}

SYSCALL_DEFINE3(recv_msg_421, unsigned long, id, unsigned char __user *, msg,
long, n){
	mailbox_t *the_mailbox = NULL;
	message_t *the_mssg = NULL;
	message_t *temp_mssg = NULL;

	if(msg == NULL){
		return -EFAULT;
	}
	if( !access_ok(msg, n)){
		return -EFAULT;
	}
	if(n < 0 ){
		return -EPERM;
	}
	/*lock as writer*/
	down_write(&mbox_stand_semaphore);
	list_for_each_entry(the_mailbox, &mbox_stand, i_link_mbox){
		if( id == the_mailbox->id){
			 
			list_for_each_entry_safe(the_mssg, 
			temp_mssg, 
			&the_mailbox->mssg_anchor,
			i_link_messages){
				if( copy_to_user(msg, the_mssg->the_mssg_itself, n) == 0 ){
					list_del(&the_mssg->i_link_messages);
					kfree(the_mssg);
					/*message copied and removed from mailbox, unlock*/
					up_write(&mbox_stand_semaphore);
					return n;
				}
				/*bad copy, release semaphore*/
				up_write(&mbox_stand_semaphore);
				return -EFAULT;
			}
		}
	}
	/*mailbox not found, release semaphore*/
	up_write(&mbox_stand_semaphore);
	return -ENOENT;
}

SYSCALL_DEFINE3(peek_msg_421, unsigned long, id, unsigned char __user *, msg,
long, n){
	mailbox_t *the_mailbox = NULL;
	message_t *the_mssg = NULL;

	if(msg == NULL){
		return -EFAULT;
	}
	if( !access_ok(msg, n)){
		return -EFAULT;
	}
	if(n < 0 ){
		return -EPERM;
	}
	/*lock mbox_stand as reader*/
	down_read(&mbox_stand_semaphore);
	list_for_each_entry(the_mailbox, &mbox_stand, i_link_mbox){
		if( id == the_mailbox->id){
		
			list_for_each_entry(the_mssg, 
			&the_mailbox->mssg_anchor,
			i_link_messages){
				
				if( copy_to_user(msg, the_mssg->the_mssg_itself, n) == 0 ){
					/*unlock*/
					up_read(&mbox_stand_semaphore);
					return n; 
				}
				/*unlock*/
				/*bad copy*/
				up_read(&mbox_stand_semaphore);
				return -EFAULT;
			}
		}
	}
	/*mailbox not found*/
	up_read(&mbox_stand_semaphore);
	return -ENOENT;
}

SYSCALL_DEFINE1(count_msg_421, unsigned long, id){
	mailbox_t *the_mailbox;
	message_t *the_mssg;
	long int temp = 0;
	
	/*lock as reader*/
	down_read(&mbox_stand_semaphore);
	list_for_each_entry(the_mailbox, &mbox_stand, i_link_mbox){
		if (id == the_mailbox->id){

			list_for_each_entry(the_mssg, 
			&the_mailbox->mssg_anchor, 
			i_link_messages){
				temp++;
			}
			/*unlock*/
			/*it is fine if no messages are found*/
			up_read(&mbox_stand_semaphore);
			return temp; 
		}
	}
	/*unlock*/
	/*it is not fine if the mailbox is not found*/
	up_read(&mbox_stand_semaphore);
	return -ENOENT;
}

SYSCALL_DEFINE1(len_msg_421, unsigned long, id){
	mailbox_t *the_mailbox;
	message_t *the_mssg;
	
	/*lock as reader*/
	down_read(&mbox_stand_semaphore);
	list_for_each_entry(the_mailbox, &mbox_stand, i_link_mbox){
		if (id == the_mailbox->id){
		
			list_for_each_entry(the_mssg, 
			&the_mailbox->mssg_anchor, 
			i_link_messages){
				return the_mssg->len; 
			}
			/*unlock*/
			/*no messages found*/
			up_read(&mbox_stand_semaphore);
			return -EFAULT;
		}
	}
	/*unlock*/
	/*mailbox not found*/
	up_read(&mbox_stand_semaphore);
	return -ENOENT;
}

SYSCALL_DEFINE1(print_mbox_421, unsigned long, id){
	mailbox_t *the_mailbox;
	message_t *the_mssg;
	long i; 
	
	/*lock as reader*/
	down_read(&mbox_stand_semaphore);
	list_for_each_entry(the_mailbox, &mbox_stand, i_link_mbox){
		if (id == the_mailbox->id){
		
			list_for_each_entry(the_mssg, 
			&the_mailbox->mssg_anchor, 
			i_link_messages){
								
				for(i = 0; i < the_mssg->len; i++){
					printk(KERN_CONT "%02x ", the_mssg->the_mssg_itself[i]);
					if(i%ONE_LINE == 0 ){
						printk(KERN_CONT "\n");
					}
				}
				printk("%s\n", "---");
			}
			/*unlock*/
			/*no messages found*/
			up_read(&mbox_stand_semaphore);
			return -EFAULT;
		}
	}
	/*unlock*/
	/*mailbox not found*/
	up_read(&mbox_stand_semaphore);
	return -ENOENT;
}
