<!--*
 * Description:  Todo list for project museum
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 15, 2023
-->
# TODO 
## General 
- [ ] separate actual surface size from framebuffer size in renderer
- [ ] Proper frametiming (using platform_sleep?)
- [ ] Audio subsystem
- [ ] Resource manager
- [ ] Write logs to file (timestamps?)
- [ ] Customizable input actions + input processing
- [ ] Block allocator
    - allocate big buffer to hold blocks of equal size
    - whenever an allocation is requested, search for a
        group of adjacent free blocks and return blocks
    - realloc checks to see if last block is fully used and
        has enough space for allocation. if it does not have
        enough space, try to allocate a group of blocks that is
        large enough to hold reallocation. if all fails, return null
- [ ] math: implement log, powf, no simd sqrtf
- [ ] format: implement usize/isize specifier so no need to cast
    ### Optimizations
- [ ] Optimize string formatting functions
    ### Bugs
- [ ] format: integer binary format should only print necessary bits
- [ ] make sure that atan actually produces good results.
    outside the -1 -> 1 range, it diverges heavily.
    it may not actually be that big of a problem but still.
    ### Enhancements
- [ ] format: more accurate float print out
## Linux
- [ ] Link testbed shared object to executable properly
- [ ] Rewrite with platform.h changes

