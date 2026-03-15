# 2048 for the Linux terminal

![2048shell screenshot](./assets/screenShots/screenShotOne.png)

## to build:
```
Install g++ and make with your favorite package manager, for example; sudo apt install g++ make

in root directory;

    make

The compiled binary will be placed in the build/ directory.
To clean build artifacts:

    make clean
```

Filepaths in 2048source.cpp need to be changed if you plan on moving the compiled file outside of the directory it is in but it is ready to compile straight out of clone.

Main will compile but throw error if text files are removed.

3nd edition of 2048 in the linux shell, adds better grid borders.
