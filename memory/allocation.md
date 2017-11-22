## physical page allocation
```
                                    ,- __get_dma_pages
                 __get_free_pages <--- __get_free_page
               /                    `- get_zeroed_page
              /
alloc_pages <-- alloc_page
`-> alloc_pages_node
    `-> __alloc_pages_node
        `-> __alloc_pages
            `-> __alloc_pages_nodemask //This is the 'heart' of the zoned buddy allocator.
                |-> prepare_alloc_pages
                `-> get_page_from_freelist
                    `-> rmqueue
                        `-> rmqueue_pcplist
                        `-> __rmqueue_smallest
                        `-> __rmqueue
                `-> __alloc_pages_slowpath
                    |-> wake_all_kswapds //__GFP_KSWAPD_RECLAIM
                    `-> get_page_from_freelist
                    `-> __alloc_pages_direct_compact
                    `-> get_page_from_freelist
                    `-> __alloc_pages_direct_reclaim //__GFP_DIRECT_RECLAIM
                    `-> __alloc_pages_direct_compact
                    `-> __alloc_pages_may_oom
                    `-> __alloc_pages_cpuset_fallback
```
## free page
```
free_page
`-> free_pages
    `-> __free_pages <- __free_page
        `-> free_hot_cold_page
            |-> free_pcp_prepare
            `-> free_one_page
        `-> __free_pages_ok
            |-> free_pages_prepare
            `-> free_one_page
                `-> __free_one_page //Freeing function for a buddy system allocator.
                    `-> TBD
```
