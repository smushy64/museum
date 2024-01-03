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
- [ ] engine: hash command line arguments
- [ ] audio subsystem
- [ ] resource/asset subsystem
- [ ] entity subsystem
- [ ] physics subsystem
- [ ] graphics: immediate ui rendering
- [ ] input: customizeable controls
- [ ] fmt: time formatting
- [ ] fmt: more accurate float formatting (grisu3 or errol?)
## Optimizations
## Bug fixes
- [ ] math: make sure atan produces good results
    - it diverges heavily outside the -1 -> 1 range

