![](https://github.com/jani-nykanen/mushrooms/blob/main/pictures/logo.png?raw=true)

## MUSHROOMS!

...is a puzzle game for MS-DOS with "beautiful" yet minimal CGA graphics.

In its current stage it is very, *very* broken. It can be beaten, but it requires using passwords. Reason: the game will eventually either freeze, crash or run out of memory (the game runs in real mode, so there isn't really room for memory leaking). Despite all my efforts, I haven't been able to find the source of the memory leaks.

This said, I recommend playing the game only on DOSBox. I do not know if the game runs on a VM, I haven't tried that yet, but I'm pretty sure it does not run on real hardware since it barely runs on DOSBox. 

-----


### Playing

See "Releases" to download the binary executable for MS-DOS.

----


### Building

See that `makefile`? It does no good, it is only used to build tools (currently only working on Linux) to convert asset files to more DOS-friendly formats. To build the code itself you need to do the following:
1) Get OpenWatcom (other compilers might not work, unless you replace `#pragma aux` with something else)
2) Create a new project with 16-bit DOS target
3) Add all the source code files (.c) to the project
4) Compile

-----

### "License"

You can do whatever you want with the code as long as you do not claim that you wrote it by yourself (not that I really care if you do), but you don't really need to give a credit, either. So in a way, use the code in any way you want, just don't claim *anyone* wrote it...

However, if you use the assets, then you must give a credit.

------


(c) 2021 Jani Nykänen
