#include <user/syscalls.h>
#include <config.h>
#include <stddef.h>


volatile unsigned int global_counter = 100;
char c;


void unterThread(){ 
	int local_counter = 0;
	while (global_counter < 150)
	{
		global_counter ++;
		local_counter ++;

		syscall_kprint_char("",c);
		syscall_kprint_uint(":",global_counter);
		syscall_get_id(" (");
		syscall_kprint_uint(":",local_counter);
		syscall_kprint(")\n");

		syscall_sleep(2);
	}
	syscall_exit();
	
}

void user_prog(void * arg) {
	int local_counter = 0;
	c = *((char *) arg);		
	test_user(c);

	syscall_thread(unterThread, &c, sizeof(c), 1);	//flag  = 1 für gleichen adressraum
	syscall_thread(unterThread, &c, sizeof(c), 1);	//flag = 0 für anderen adressraum

	while (global_counter < 150)
		{
			
			global_counter ++;
			local_counter ++;

			syscall_kprint_char("",c);
			syscall_kprint_uint(":",global_counter);
			syscall_get_id(" (");
			syscall_kprint_uint(":",local_counter);
			syscall_kprint(")\n");

			syscall_sleep(2);
		}
	syscall_exit();
}

void init() {
	for(;;) {
		char c = syscall_getc();
		syscall_thread(user_prog, &c, sizeof(c), 0);
	}
}


