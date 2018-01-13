## rename foo bar procedrue
1. lookup bar: NFS lookup return -2
2. lookup foo: NFS lookup return 0
3. get attr: NFS revalidating
4. nfs_lookup_revalidate: foo & bar
5. rename foo bar
