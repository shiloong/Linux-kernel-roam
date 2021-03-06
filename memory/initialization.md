## memory init 
```c
start_kernel
-> setup_arch
   -> setup_machine_fdt
      -> early_init_dt_scan
         -> early_init_dt_scan_nodes
            -> early_init_dt_scan_memory
               -> early_init_dt_add_memory_arch <- reserve_regions <- efi_init <- setup_arch
                  -> memblock_add // add physical memory info
   -> arm64_memblock_init
      -> memblock_add
   -> paging_init
      -> map_kernel
      -> map_mem
   -> bootmem_init
      -> zone_sizes_init
         -> free_area_init_node
            -> free_area_init_core //Set up the zone data structures
               -> memmap_init ~ memmap_init_zone
                  -> set_pageblock_migratetype
               -> setup_usemap
                  -> usemap_size
-> build_all_zonelists
   -> build_all_zonelists_init
      -> __build_all_zonelists
         -> build_zonelists
            -> build_zonelists_in_node_order
            -> build_thisnode_zonelists
-> mm_init
   -> mem_init
      -> free_all_bootmem
         -> free_low_memory_core_early (free_all_bootmem_core:bootmem)
            -> __free_memory_core
               -> __free_pages_memory
                  -> __free_pages_bootmem
                     -> __free_pages_boot_core
                        -> __free_pages // fillup buddy system
```
