<!--
 * Description:  Todo for Liquid Engine/Project Museum
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: September 29, 2023
-->
# TODO
## Common
- [x] memory: block allocator
    - allocates memory in equally sized blocks
    - returns pointer to a range of blocks big enough to
        hold requested memory
- [x] logging: write logs to file (with timestamps?)
- [x] fmt: len argument for pointer argument
    - ex:
    u8 foo[2] = { 1, 2 };
    println( "{u8,*_}", 2, foo );
    "{ 1, 2 }"
- [x] fmt: repeat argument for char
    - ex:
    println( "'{c}' '{c,r4}'", 'x', 'x' );
    "'x' 'xxxx'"
    println( "{c,r_}", 4, 'x' );
    "xxxx"
- [ ] engine: hash command line arguments
- [ ] audio subsystem
- [ ] resource/asset subsystem
- [ ] entity subsystem
- [ ] physics subsystem
- [ ] graphics: immediate ui rendering
- [ ] input: customizeable controls
- [ ] fmt: time formatting
- [ ] fmt: more accurate float formatting (grisu3 or errol?)
- [ ] fmt: KValue formatting
- [ ] fmt: display NULL when const char* is null
- [ ] fmt: PathSlice formatting argument
    - / to change all slashes to forward
    - \ to change all slashes to back
## Optimizations
- [ ] collections: hash SIMD lookup
## Bug fixes
- [x] fmt: len argument for pointer argument was failing end brace check
    - fix: advance 'at' pointer after checking for _
- [ ] math: make sure atan produces good results
    - it diverges heavily outside the -1 -> 1 range

