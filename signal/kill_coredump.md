# syscall kill
```c
kill
`-> kill_something_info
    `-> kill_pid_info
        `-> group_send_sig_info                                       <- tkill
	    |-> check_kill_permission <- do_send_specific <- do_tkill <- tgkill
	    `-> do_send_sig_info      <-'
	        `-> send_signal
		    `-> __send_signal
		        `-> complete_signal
			    `-> signal_wake_up
			        `-> signal_wake_up_state
				    |-> set_tsk_thread_flag
				    `-> kick_process
```

# coredump
```c
ret_to_user_from_irq
`-> slow_work_pending
    `-> do_work_pending
        `-> do_signal
            `-> get_signal
                `-> do_coredump
```
