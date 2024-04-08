# CVector

## Table of Contents
 * [About](#about) 
 * [Building from source](#building-from-source)
 * [Linking](#linking)
   * [GNU](#gnu)
   * [MSVC](#msvc)
   * [TurboC](#turboc)
 * [Common issues](#common-issues)
 * [Usage](#usage)

----------
## About
A cross-platform library, written in `C`, to bring `std::vector` functionality 
from `C++` to `C`. The library was written in the `ANSI C` standard with only
the `C` Standard Library. Meaning it should be compatible with anything that
can compile C. It has even been tested in MS-DOS, and works beautifully.

----------

## Building from source
To build the library from source, you first need to copy and rename the
requisite makefile, located in the `makefiles` directory, based on your build
system from `makefile.xxx` to `makefile`, and place it in the root of this
directory. Then, open your terminal/command prompt and type one of the
following. If you are using the `MSVC` makefile, swap `make` for `nmake`:
```bash
# Build both the static and shared library
make

# Build just the static library
make staticlib

# Build just the shared library
make sharedlib
```
> [!NOTE]
> The `sharedlib` variant is only compatible with the `GNU` makefile. It is
> **NOT** supported under the `MSVC` or `TurboC` makefile

You can also build the provided test program by typing the following:
```bash
# Build the test program linking against the shared library
make test-shared

# Build the test program linking against the static library
make test-static
```
> [!NOTE]
> The `test-shared` variant is only compatible with the `GNU` makefile. It is
> **NOT** supported under the `MSVC` or `TurboC` makefile

The libraries also have debug variants<sup>*</sup> that can be built by
appending `DEBUG=yes` to your make command
```bash
# Builds both the static and shared debug versions of the library
make DEBUG=yes 

# Builds the test program, linking against the static library, in
# debug mode, using the debug library
make test-static DEBUG=yes
```
**For `MSVC` ONLY:** To build the release version, add `RELEASE=yes`

<sup>*</sup>: The TurboC makefile does not have a native debug version. 
However, you can create one by changing the `CFLAGS` to be `-w -c -v -y`
then rebuilding the library.

----------
## Linking
### GNU
In order to properly link, you have to add `-lcvector` to your linked 
libraries as well as the path it is located in. Here is the example
from the provided test program:
```bash
# Linking against the static library
LIBS = -L 'libs/release/static -lcvector'

# Linking against the shared library
LIBS = -L 'libs/release/shared -lcvector'
```
#### Linking against the shared library
When linking with the shared library on macOS and GNU/Linux, 
you need to add an `rpath` to point to the location of the 
shared library by adding an `LDFLAG`. Again, here is the example
from the provided test program:
```bash
LDFLAGS = -Wl,-rpath 'libs/release/shared'
```
Doing this will ensure your program can actually find the library 
when you try to run it.

On Windows, make sure the `dll` is in the same directory as your 
executable.

### MSVC
To link against the static library using the `MSVC` compiler append the
library name to the end of your command.
```batch
cl /MD /O2 example.c /Fe example.exe cvector.lib
```

### TurboC
To link against the static library using the `TurboC` compiler append the
library name to the end of your command.
```batch
tcc -w -G -eexample.exe example.c cvector.lib
```


----------
## Common issues
Linking with the static library is fairly straight forward. 
However, linking with the shared library can pose issues. 

### -lcvector not found
If you run into an error message saying:
```
ld: library not found for -lcvector
# OR
/usr/bin/ld: cannot find -lcvector: No such file or directory
```

The reason this is happening is due to the linker being unable to
find the `libcvector.so` or `libcvector.dylib` file respectively. 
Most likely, the cause is that you only have the `libcvector.so.x.x.x` or 
`libcvector.x.x.x.dylb` in the folder you specified with your `-L`
flag. To fix this, create a symbolic link as seen below:
```bash
# GNU/Linux
ln -s libcvector.so.x.x.x libcvector.so

# macOS
ln -s libcvector.x.x.x.dylb libcvector.dylb
```
Alternatively, rather than passing `-L /path/to/lib -lcvector`, you
could just give the path to the shared library, as seen below:
```bash
# Errors saying it can't find -lcvector
gcc example.c -o example -lcvector -L libs -Wl,-rpath 'libs'

# No errors
gcc example.c -o example libs/libcvector.so.1.0.0 -Wl,-rpath 'libs'
```

### Code compiles, but unable to find the shared library
Similar to the error above, if your code compiles but you get a message
saying something to the effect of it can't find `libcvector.so.x.x.x`
or `libcvector.x.x.x.dylb`, this is due to you not having said file in
your `rpath`. To fix this, make sure the directory you are specifying 
as your `rpath` contains `libcvector.so.x.x.x` or `libcvector.x.x.x.dylb`
depending on your platform.

----------
## Usage
Usage of the library has three components, linking (which we discussed
above), including, and calling the functions.

### Including
First, ensure you are including the header file `vector.h` in your program.
```c
#include "vector.h"
```
You also need to ensure the directory where you put the `vector.h` file is 
included in your project. For example, if you have a `headers/` folder 
where you added `vector.h`, you would need `-I headers/` in your makefile.

### Using in your program
The library includes many of the functions found in `std::vector`. The main
difference is they are called with the prefix `vect_` followed by the
function with a pointer to the vector you wish to operate on as the first
parameter. For instance `push_back` looks like this:
```c
int val = 42;
vect_push_back(&v, &val);
```
> [!IMPORTANT]  
> Make sure you call `vect_init` **BEFORE** using the vector. 
> Failing to do so can lead to undefined behavior.


### Example code
```c
#include <stdio.h>

#include "vector.h"

int main(void)
{
    /* Initialize a vector of ints with an initial size of 5 */
    vector v = vect_init(5, sizeof(int));
    int x, val = 10;

    /* Add elements to the vector */
    for (x = 0; x < 8; x++)
        vect_push_back(&v, &x);

    /* Insert val (10) at index 3 */
    vect_insert(&v, &val, 3);
    
    /* Remove the last element */
    vect_pop_back(&v);

    /* Print out the vector */
    for (x = 0; x < (int) vect_size(&v); x++)
        printf("v[%d]: %d\n", x, *(int *) vect_at(&v, x));

    /* Free the memory held by the vector */
    vect_free(&v);

    return 0;
}
```
**Output:**
```
v[0]: 0
v[1]: 1
v[2]: 2
v[3]: 10
v[4]: 3
v[5]: 4
v[6]: 5
v[7]: 6
```

Assuming you called the program `example.c` and had `vector.h` and that 
static version of the library in the same directory,
it could be compiled with the following command:
```bash
# GCC
gcc example.c -o example -L ./ -lcvector
# MSVC
cl example.c /Fe example.exe cvector.lib
# TurboC
tcc -eexample.exe example.c cvector.lib
```
