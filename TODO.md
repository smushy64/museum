<!--*
 * Description:  Todo list for project museum
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 15, 2023
-->
# TODO 
## General 
- [x] rewrite build system to make it easier to work with
- [x] separate actual surface size from framebuffer size in renderer
- [x] math: separate functions into source file instead of header-only lib.
- [x] math: implement log, powf
- [x] format: implement usize/isize specifier so no need to cast
- [x] defines: change all macro function names to lower case
- [x] defines: move constants out to a separate file
- [ ] renderer: complete rewrite
- [ ] proper frametiming (using platform_sleep?)
- [ ] audio subsystem
- [ ] resource manager
- [ ] write logs to file (timestamps?)
- [ ] customizable input actions + input processing
- [ ] block allocator
    - allocate big buffer to hold blocks of equal size
    - whenever an allocation is requested, search for a
        group of adjacent free blocks and return blocks
    - realloc checks to see if last block is fully used and
        has enough space for allocation. if it does not have
        enough space, try to allocate a group of blocks that is
        large enough to hold reallocation. if all fails, return null
    ### Optimizations
- [ ] optimize string formatting functions
    ### Bugs
- [x] format: integer binary format should only print necessary bits
- [x] format: integer 0 not getting formatted
- [x] mathf: transform_world_matrix has local and parent switched
- [ ] make sure that atan actually produces good results.
    outside the -1 -> 1 range, it diverges heavily.
    it may not actually be that big of a problem but still.
    ### Enhancements
- [ ] format: more accurate float print out (grisu3? or maybe errol)

