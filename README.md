# brrUtils

a simple BRR encoding/decoding library.

# usage

copy brrUtils.c and brrUtils.h to your project (remember to add brrUtils.c to your sources list).

include brrUtils.h. it provides two functions:

`brrEncode()` for encoding a 16-bit PCM sample to BRR, and

`brrDecode()` for decoding a BRR sample to 16-bit PCM.

read brrUtils.h for notes and more information.

# brrenc/brrdec

if you compile the CMake project, you'll get two executables: brrenc and brrdec.
these can be used to do a quick command-line conversion from/to BRR.
