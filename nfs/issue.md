## ISSUE
command 'mv'(rename) return ENOENT on NFS client, but the target file has been
renamed successful on NFS server.

## ENV
Kernel version: 3.10.64--sampleversion-lcpa #5 SMP PREEMPT
Systemd: ExecStart=/usr/sbin/rpc.nfsd -V 2 -V 3 -V 4 -V 4.1 3

192.168.253.16:/pns/vendorFM/p/lcp0 on /mwFm/log type nfs4 (rw,relatime,vers=4.0,rsize=1048576,wsize=1048576,namlen=255,acregmin=0,acregmax=0,acdirmin=0,acdirmax=0,hard,proto=tcp,port=0,timeo=100,retrans=2,sec=sys,clientaddr=192.168.253.89,local_lock=none,addr=192.168.253.16)

192.168.129.2:/pns/sdm_a on /sdm_a type nfs4 (rw,relatime,vers=4.0,rsize=1048576,wsize=1048576,namlen=255,hard,proto=tcp,port=0,timeo=600,retrans=2,sec=sys,clientaddr=192.168.253.6,local_lock=none,addr=192.168.129.2)



## TEST
run the test code `whack_rename_on_nfs`, get error log.

## TEST result:
```
# ./whack_rename_on_nfs 100
WHACK: create file 'f100_0'
WHACK: rename 1000000 times from foo#i to foo#(i+1)...
rename failed: No such file or directory
errno: 2
arg1: "f100_7346"
arg2: "f100_7347"
WHACK: try to rename back immediately...
rename failed: No such file or directory
errno: 2
arg1: "f100_7347"
arg2: "f100_7346"
```

## logs
1. enable the rpc/nfsd log by `rpcdebug -m rpc -s all` and `rpcdebug -m nfsd -s all`.

2. add more log in nfsd.
```
# rename success, but sendto failed:
[ 1199.385058] [nfsd-1117] svc: calling dispatcher
[ 1199.385080] [nfsd-1117] nfsv4 compound op #1/4: 22 (OP_PUTFH)
[ 1199.385097] [nfsd-1117] nfsv4 compound op 800000019e0e1080 opcnt 4 #1: 22: status 0
[ 1199.385104] [nfsd-1117] nfsv4 compound op #2/4: 32 (OP_SAVEFH)
[ 1199.385111] [nfsd-1117] nfsv4 compound op 800000019e0e1080 opcnt 4 #2: 32: status 0
[ 1199.385118] [nfsd-1117] nfsv4 compound op #3/4: 22 (OP_PUTFH)
[ 1199.385132] [nfsd-1117] nfsv4 compound op 800000019e0e1080 opcnt 4 #3: 22: status 0
[ 1199.385138] [nfsd-1117] nfsv4 compound op #4/4: 29 (OP_RENAME)
[ 1199.387766] [nfsd-1117] nfsd_rename: f102_0 -> f102_1: err=0x0
[ 1199.387780] [nfsd-1117] nfsv4 compound op 800000019e0e1080 opcnt 4 #4: 29: status 0
[ 1199.387787] [nfsd-1117] nfsv4 compound returned 0
[ 1199.387813] [nfsd-1117] sendto failed?

# lookup failure cause TCP link down:
[ 1199.378899] [nfsd-1119] svc: calling dispatcher
[ 1199.378909] [nfsd-1119] nfsv4 compound op #1/4: 22 (OP_PUTFH)
[ 1199.378928] [nfsd-1119] nfsv4 compound op 8000000199078080 opcnt 4 #1: 22: status 0
[ 1199.378936] [nfsd-1119] nfsv4 compound op #2/4: 15 (OP_LOOKUP)
[ 1199.378956] [nfsd-1119] nfsv4 compound op 8000000199078080 opcnt 4 #2: 15: status 2
[ 1199.378963] [nfsd-1119] nfsv4 compound returned 2
[ 1199.379251] [nfsd-1119] shutdown? return SVC_CLOSE here!
[ 1199.385072] [nfsd-1119] SVC_CLOSE
[ 1199.385086] [nfsd-1119] SVC_DROP
[ 1199.385092] [nfsd-1119] svc: svc_process dropit w/o send ACK
[ 1199.385302] [nfsd-1119] svc_drop

# rename again and failed
[ 1199.385683] [nfsd-1119] svc: calling dispatcher
[ 1199.385698] [nfsd-1119] nfsv4 compound op #1/4: 22 (OP_PUTFH)
[ 1199.385717] [nfsd-1119] nfsv4 compound op 8000000199078080 opcnt 4 #1: 22: status 0
[ 1199.385725] [nfsd-1119] nfsv4 compound op #2/4: 32 (OP_SAVEFH)
[ 1199.385733] [nfsd-1119] nfsv4 compound op 8000000199078080 opcnt 4 #2: 32: status 0
[ 1199.385740] [nfsd-1119] nfsv4 compound op #3/4: 22 (OP_PUTFH)
[ 1199.385754] [nfsd-1119] nfsv4 compound op 8000000199078080 opcnt 4 #3: 22: status 0
[ 1199.385761] [nfsd-1119] nfsv4 compound op #4/4: 29 (OP_RENAME)
[ 1199.387451] [nfsd-1119] nfsd_rename: f102_0 -> f102_1: err=0x2
[ 1199.387483] [nfsd-1119] nfsv4 compound op 8000000199078080 opcnt 4 #4: 29: status 2
[ 1199.387501] [nfsd-1119] nfsv4 compound returned 2
```

## FIX
https://www.spinics.net/lists/linux-nfs/msg37525.html
