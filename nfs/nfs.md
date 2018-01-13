##Server: nfsd
### Kernel version: v3.10.64
```
nfsd
|-> svc_recv
    |-> svc_alloc_arg
    |-> try_to_freeze
    |-> svc_get_next_xprt
    |-> svc_handle_xprt
        |-> xprt->xpt_ops->xpo_recvfrom = svc_tcp_recvfrom
            |-> svc_tcp_recv_record
                |-> svc_recvfrom

|-> svc_process
    |-> svc_process_common
        |-> auth_res = svc_authenticate(rqstp, &auth_stat);
            |-> dprintk("svc: svc_authenticate (%d)\n", flavor);
            |-> aops->accept(rqstp, authp); = svcauth_unix_accept
        |-> auth_res = progp->pg_authenticate(rqstp); = &lockd_authenticate
            |-> svc_set_client
                |-> rqstp->rq_authop->set_client(rqstp); = svcauth_unix_set_client
                    |-> ip_map_cached_get
                        |-> ipm = xprt->xpt_auth_cache;
                    |-> __ip_map_lookup
                        |-> sunrpc_cache_lookup
                            |-> ??
                    |-> cache_check
                        |-> cache_is_valid
                        |-> dprintk("RPC:       Want update, refage=%ld, age=%ld\n",
                    |-> unix_gid_find => -ESHUTDOWN
                        |-> cache_check => -ETIMEDOUT
                            |-> cache_is_valid => -EAGAIN
                            |-> cache_defer_req => false
                            |-> cache_is_valid => -EAGAIN
        ?-> SVC_CLOSE
            |-> svc_close_xprt
                |-> svc_delete_xprt
                    |-> xprt->xpt_ops->xpo_detach(xprt); = svc_tcp_sock_detach
                        |-> svc_sock_detach
                    |-> svc_xprt_put
                        |-> svc_xprt_free
                            |-> xprt->xpt_ops->xpo_free(xprt); = svc_sock_free
                                |-> sockfd_put
                                |-> sock_release

        ?-> SVC_DROP
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
    ?-> svc_send
        |-> rqstp->rq_xprt->xpt_ops->xpo_release_rqst
        |-> xprt->xpt_ops->xpo_sendto = svc_tcp_sendto
            |-> svc_sendto
                |-> sock_sendmsg
        |-> FIXME: how to handle send failure?
            |-> In case the compound has been executed completely?
            |-> It should send the ACK to client by retry here when the connect is re-build.

        |-> rpc_wake_up
        |-> svc_xprt_release

    ?-> svc_drop
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
                                    |-> call_connect_status
                                =-> call_transmit
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
                                        |-> rpc_sleep_on
                                            |-> __rpc_sleep_on_priority
                                                |-> __rpc_add_wait_queue
                                                |-> __rpc_add_timer
                                    |-> call_transmit_status
                                    |-> rpc_wake_up_queued_task
                                =-> rpc_exit_task
                                    |-> task->tk_ops->rpc_call_done =
                                    |-> xprt_release

                                |-> rpc_set_running
                                |-> dprintk("RPC: %4u sync task resuming\n",
                                <-> for
                                |-> dprintk("RPC: %5u return %d, status %d\n",
                                |-> rpc_release_task
```
