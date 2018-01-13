## sig send
```
force_sig
|-> force_sig_info
    |-> force_sig_info(sig, SEND_SIG_PRIV, p);
        |-> specific_send_sig_info
            |-> send_signal
                |-> __send_signal
                    |-> case (unsigned long) SEND_SIG_PRIV:
                             q->info.si_signo = sig;
                             q->info.si_errno = 0;
                             q->info.si_code = SI_KERNEL;
                             q->info.si_pid = 0;
                             q->info.si_uid = 0;
                             break;
                    |-> complete_signal

```
