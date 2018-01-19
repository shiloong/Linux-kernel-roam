##Server: nfsd
### Kernel version: v3.10.64
```
create nfsd thread:
/proc/fs/nfsd/threads
write_threads
|-> nfsd_svc
    |-> nfsd_create_serv
        |-> nfsd_reset_versions
        |-> svc_create_pooled
        |-> svc_bind
        |-> do_gettimeofday
    |-> nfsd_startup_net
        |-> nfsd_startup_generic
        |-> nfsd_init_socks
            |-> svc_create_xprt: udp
            |-> svc_create_xprt: tcp
                |-> dprintk("svc: creating transport %s[%d]\n", xprt_name, port);
                |-> __svc_xpo_create
                    |-> xcl->xcl_ops->xpo_create = svc_tcp_create
                        |-> svc_create_socket
                            |-> dprintk("svc: svc_create_socket(%s, %d, %s)\n",
                            |-> __sock_create
                            |-> svc_reclassify_socket
                            |-> kernel_bind
                            |-> kernel_getsockname
                            |-> svc_setup_socket: <struct svc_xprt>
                                |-> svc_setup_socket
                                    |-> dprintk("svc: svc_setup_socket %p\n", sock);
                                    |-> svc_sock_setbufsize
                                    |-> svc_tcp_init
                                    |-> dprintk("svc: svc_setup_socket created %p (inet %p)\n",
                            |-> svc_xprt_set_local
                |-> svc_add_new_perm_xprt
                |-> svc_xprt_local_port
                    |-> svc_xprt_local_port
        |-> lockd_up
        |-> nfs4_state_start_net
    |-> svc_set_num_threads
|-> nfsd_nrthreads
```

```
nfsd
|-> svc_recv
    |-> svc_alloc_arg
    |-> try_to_freeze
    |-> svc_get_next_xprt
    |-> svc_handle_xprt
        ?-> test_bit(XPT_LISTENER, &xprt->xpt_flags)
        T-> xprt->xpt_ops->xpo_accept = svc_tcp_accept
            |-> dprintk("svc: tcp_accept %p sock %p\n", svsk, sock);
            |-> dprintk("%s: connect from %s\n", serv->sv_name,
            |-> svc_setup_socket
        F-> xprt->xpt_ops->xpo_recvfrom = svc_tcp_recvfrom
            |-> svc_tcp_recv_record
                |-> svc_recvfrom

|-> svc_process
    |-> svc_process_common
        |-> auth_res = svc_authenticate(rqstp, &auth_stat);
            |-> dprintk("svc: svc_authenticate (%d)\n", flavor);
            |-> aops->accept(rqstp, authp); = svcauth_unix_accept
        ?-> auth_res = progp->pg_authenticate(rqstp) = svc_set_client
            |-> rqstp->rq_authop->set_client(rqstp) = svcauth_unix_set_client
                |-> ip_map_cached_get
                |   |-> ipm = xprt->xpt_auth_cache;
                |-> __ip_map_lookup
                |   |-> sunrpc_cache_lookup
                |       |-> ??
                |-> cache_check
                |   |-> cache_is_valid
                |   |-> dprintk("RPC:       Want update, refage=%ld, age=%ld\n",
                |   ?-> test_and_set_bit(CACHE_PENDING, &h->flags)
                |   F-> cache_make_upcall
                |       |-> sunrpc_cache_pipe_upcall
                |   I-> try_to_negate_entry
                |       |-> cache_fresh_unlocked
                |           |-> cache_revisit_request
                |           |-> cache_dequeue
                |   A-> cache_fresh_unlocked
                |-> unix_gid_find => -ESHUTDOWN
                    |-> cache_check => -ETIMEDOUT
                        |-> cache_is_valid => -EAGAIN
                        |-> cache_defer_req => false
                        |-> cache_is_valid => -EAGAIN
        :-> SVC_CLOSE
            |-> svc_close_xprt
                |-> svc_delete_xprt
                    |-> xprt->xpt_ops->xpo_detach(xprt); = svc_tcp_sock_detach
                        |-> svc_sock_detach
                    |-> svc_xprt_put
                        |-> svc_xprt_free
                            |-> xprt->xpt_ops->xpo_free(xprt); = svc_sock_free
                                |-> sockfd_put
                                |-> sock_release

        :-> SVC_DROP
            |-> svc_authorise(rqstp);
            |-> dprintk("svc: svc_process dropit\n");

        |-> dprintk("svc: calling dispatcher\n");
        |-> versp->vs_dispatch(rqstp, statp) = nfsd_dispatch
            |-> nfsd_cache_lookup
            |-> proc->pc_func = nfsd4_proc_compound
                >-> while (!status && resp->opcnt < args->opcnt) {
                |-> dprintk("nfsv4 compound op #%d/%d: %d (%s)\n",
                |-> opdesc->op_func = (nfsd4op_func)nfsd4_rename
                    |-> nfsd_rename
                        |-> vfs_rename
                |-> dprintk("nfsv4 compound op %p opcnt %d #%d: %d: status %d\n",
                <-> while

            |-> map_new_errors
            |-> nfsd_cache_update

        |-> procp->pc_release
        |-> sendit:
                 if (svc_authorise(rqstp))
                        goto dropit;
                 return 1;
    1-> svc_send
        |-> rqstp->rq_xprt->xpt_ops->xpo_release_rqst
        |-> xprt->xpt_ops->xpo_sendto = svc_tcp_sendto
            |-> svc_sendto
                |-> sock_sendmsg
        |-> XXX: this could cause a issue that NFS client retry will get wrong result!

        |-> rpc_wake_up
        |-> svc_xprt_release

    0-> svc_drop
        |-> svc_xprt_release
            |-> rqstp->rq_xprt->xpt_ops->xpo_release_rqst = svc_release_skb
                |-> skb_free_datagram_locked
                    |-> skb_orphan
            |-> svc_reserve
```

##Client: nfsv4
```
vfs_rename
|-> nfs_rename
    |-> dfprintk(VFS, "NFS: rename(%s/%s -> %s/%s, ct=%d)\n"
    |-> nfs_sillyrename
    |-> NFS_PROTO(old_dir)->rename = nfs4_proc_rename: <retry>
        |-> nfs4_handle_exception
        |-> _nfs4_proc_rename
            |-> nfs4_call_sync
                |-> server->nfs_client->cl_mvops->call_sync = nfs4_call_sync_sequence
                    |-> rpc_run_task
                        |-> rpc_new_task
                            |-> rpc_init_task
                                |-> dprintk("RPC:       new task initialized, procpid %u\n",
                            |-> dprintk("RPC:       allocated task %p\n", task);
                        |-> rpc_task_set_client
                        |-> rpc_task_set_rpc_message
                        |-> rpc_call_start
                            |-> task->tk_action = call_start

                        |-> rpc_execute
                            |-> __rpc_execute
                                |-> dprintk("RPC: %5u __rpc_execute flags=0x%x\n"
                                >-> for (;;):
                                |-> do_action = task->tk_action;
                                =-> call_start
                                    |-> dprintk("RPC: %5u call_start %s%d proc %s (%s)\n",
                                =-> call_reserve
                                    |-> xprt_reserve
                                        |-> xprt->ops->alloc_slot = xprt_lock_and_alloc_slot
                                            |-> xprt_alloc_slot
                                                |-> xprt_request_init
                                                    |-> req->rq_timeout = task->tk_client->cl_timeout->to_initval;
                                                    |-> req->rq_xid = xprt_alloc_xid
                                                    |-> xprt_reset_majortimeo
                                                    |-> dprintk("RPC: %5u reserved req %p xid %08x\n",
                                =-> call_reserveresult
                                =-> call_refresh;
                                    |-> rpcauth_refreshcred
                                =-> call_refreshresult
                                =-> call_allocate
                                    |-> xprt->ops->buf_alloc
                                =-> call_bind
                                    |-> xprt->ops->rpcbind
                                =-> call_connect
                                    |-> dprintk("RPC: %5u call_connect xprt %p %s connected\n",
                                    |-> xprt_connect
                                        |-> dprintk("RPC: %5u xprt_connect xprt %p %s connected\n",
                                    |-> call_connect_status
                                =-> call_transmit //5.   Transmit the RPC request, and wait for reply
                                    |-> xprt_prepare_transmit
                                        |-> dprintk("RPC: %5u xprt_prepare_transmit\n",
                                    |-> rpc_xdr_encode
                                        |-> rpc_encode_header
                                            |-> rpcauth_marshcred
                                        |-> rpcauth_wrap_req
                                    |-> xprt_transmit
                                        |-> dprintk("RPC: %5u xprt_transmit(%u)\n",
                                        |-> xprt->ops->send_request = xs_tcp_send_request
                                            |-> dprintk("RPC:       xs_tcp_send_request(%u) = %d\n",
                                            |-> xs_sendpages
                                                |-> xs_send_pagedata
                                                    |-> do_sendpage = sock->ops->sendpage
                                                        |-> sock_sendpage
                                        |-> dprintk("RPC: %5u xmit complete\n",
                                        |-> xprt->ops->set_retrans_timeout = xprt_set_retrans_timeout_def
                                            |-> 
                                        |-> rpc_sleep_on //we're expecting a reply
                                            |-> __rpc_sleep_on_priority
                                                |-> dprintk("RPC: %5u sleep_on(queue \"%s\" time %lu)\n",
                                                |-> __rpc_add_wait_queue
                                                |-> __rpc_add_timer
                                    |-> call_transmit_status
                                    |-> rpc_wake_up_queued_task

                                <got reply data>
                                sk->sk_data_ready = xs_tcp_data_ready //"data ready" callback for TCP sockets
                                |-> xprt_from_sock
                                |-> tcp_read_sock(sk, &rd_desc, xs_tcp_data_recv);
                                    |-> dprintk("RPC:       xs_tcp_data_recv started\n");
                                    |-> xs_tcp_read_fraghdr
                                    |-> xs_tcp_read_xid
                                    |-> xs_tcp_read_calldir
                                    |-> xs_tcp_read_data
                                    |-> xs_tcp_read_discard
                                    |-> dprintk("RPC:       xs_tcp_data_recv done\n");

                                =-> call_status
                                =-> call_decode
                                    |-> dprintk("RPC: %5u call_decode result %d\n",

                                =-> rpc_exit_task
                                    |-> task->tk_ops->rpc_call_done =
                                    |-> xprt_release

                                |-> rpc_set_running
                                |-> dprintk("RPC: %4u sync task resuming\n",
                                <-> for
                                |-> dprintk("RPC: %5u return %d, status %d\n",
                                |-> rpc_release_task
```
