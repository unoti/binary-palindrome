# binary-palindrome
Approaches for finding binary palindromes in C.

## Introduction
A *binary palindrome* is a number like ```00011000``` which has the same bit stream both forward and backward.  This repo looks at different methods of determining whether a number is a binary palindrome and compares the speed of the approaches.

First we'll make a container for building the code, then we'll try a few of approaches.

## Container
I'm basing my container on this [small docker container with gcc from Docker-Hub-frolvlad](https://github.com/Docker-Hub-frolvlad/docker-alpine-gcc/blob/master/Dockerfile).

I'm adding **bash** to the container.


There are two files related to the container:
* **Dockerfile**.  This builds the container image with the C compiler.
* **docker-compose.yml** This merges the container with environment information.  It mounts a subdirectory of the local host machine into the container.  This way we can recompile without restarting the container.

Notice under ```volumes``` we have this:
```yml
volumes:
    code:
        driver: local
        driver_opts:
            type: none
            device: '/host_mnt/d/code/lab/binary-palindrome'
            o: bind
```

The ```device``` section specifies what directory on my Windows machine to mount on the container.  Note that ```/host/mnt/d/``` under Docker for Windows is how you specify to connect a folder in drive D:\ to the container.

To connect to the container from a Windows system:
```
winpty docker-compose run sys bash
cd /code/src
```

## Compile
First start the container as described above, and do ```cd /code/src```.
Then:
```
make
```

For iterative development, you might use something like:
```
clear && make && ./palindrome 1
```

# Approaches
## 1. Naive
```c
int is_palindrome_naive(unsigned long num) {
    // determine if num is a palindrome.
    unsigned long left_mask = 1 << (BITS-1);
    unsigned long right_mask = 1;

    while (left_mask) {
        if ( ((num & left_mask) !=0) != ((num & right_mask) != 0) )
            return FALSE;
        left_mask >>= 1;
        right_mask <<= 1;
    }
    return TRUE;
}
```
## 2. Unrolled Ands
```c
int is_palindrome_ands(unsigned long num) {
    // determine if num is a palindrome.
    return (
        (((num & 0x80) != 0) == ((num & 0x01) != 0)) &&
        (((num & 0x40) != 0) == ((num & 0x02) != 0)) &&
        (((num & 0x20) != 0) == ((num & 0x04) != 0)) &&
        (((num & 0x10) != 0) == ((num & 0x08) != 0))
    );
}
```
The above version only handles 8-bit palindromes.  We must modify this code to handle palindromes of different sizes.  My guess by comparing those two side-by-side is that the optimizer will unroll the naive version to be similar to the unrolled version, and that their performance should be approximately identical.  To find out, I'm going to have to modify the code a lot to make a 30-bit version, which could take me a while to do without making any mistakes!

Here's a more complete version for 16 bits:
```c
int is_palindrome_ands16(unsigned long num) {
    // determine if num is a palindrome.
    return (
        (((num & 0x80000000) != 0) == ((num & 0x00000001) != 0)) &&
        (((num & 0x40000000) != 0) == ((num & 0x00000002) != 0)) &&
        (((num & 0x20000000) != 0) == ((num & 0x00000004) != 0)) &&
        (((num & 0x10000000) != 0) == ((num & 0x00000008) != 0)) &&
        (((num & 0x08000000) != 0) == ((num & 0x00000010) != 0)) &&
        (((num & 0x04000000) != 0) == ((num & 0x00000020) != 0)) &&
        (((num & 0x02000000) != 0) == ((num & 0x00000040) != 0)) &&
        (((num & 0x01000000) != 0) == ((num & 0x00000080) != 0)) &&
        (((num & 0x00800000) != 0) == ((num & 0x00000100) != 0)) &&
        (((num & 0x00400000) != 0) == ((num & 0x00000200) != 0)) &&
        (((num & 0x00200000) != 0) == ((num & 0x00000400) != 0)) &&
        (((num & 0x00100000) != 0) == ((num & 0x00000800) != 0)) &&
        (((num & 0x00080000) != 0) == ((num & 0x00001000) != 0)) &&
        (((num & 0x00040000) != 0) == ((num & 0x00002000) != 0)) &&
        (((num & 0x00020000) != 0) == ((num & 0x00004000) != 0)) &&
        (((num & 0x00010000) != 0) == ((num & 0x00008000) != 0))
    );
}
```

## 3. Byte pattern matching
1. Build a table 256 elements long of the palindome value for each byte
2. Check if a 32 bit value is a palindrome like this:
```c
int is_palindrome_pattern32(unsigned long num) {
    unsigned char b3 = (num >> 24);
    unsigned char b2 = (num >> 16) & 0xff;
    unsigned char b1 = (num >> 8) & 0xff;
    unsigned char b0 = num & 0xff;
    return b0 == byte_palindromes[b3] &&
        b1 == byte_palindromes[b2];
}
```

## 4. The Skipper
The idea of this approach is to just build the palindromes explicitly rather than iterating over all the possible numbers and checking each one.

To illustrate, let's look at how we'd generate the palindromes for 8 bits.  There are 16 palindromes for 8 bit numbers.

## Examining Binary Palindromes
Let's run a quick test with the naive implementation for 8 bits.
```c
#define BITS 8 // Number of bits we'll check palindromes for.
```
```
clear && make && ./palindrome 1
```

After writing the naive implementation and running it for 8 bits, we get this result:

```
cc -O3 -c palindrome.c
cc -O3 -o palindrome palindrome.o
Binary Palindromes
Palindrome - naive
0 00000000
18 00011000
24 00100100
3c 00111100
42 01000010
5a 01011010
66 01100110
7e 01111110
81 10000001
99 10011001
a5 10100101
bd 10111101
c3 11000011
db 11011011
e7 11100111
ff 11111111
16 palindromes found with respect to 8 bits.
bash-5.0#
```

Notice the patterns in here. Looking at nybbles (half-bytes), there are certain pairs of nybbles that have their palindromes.  0x1 (0001) is paired with 0x8 (1000).  0x2 (0010) is paired with 0x4 (0100), and so on.

As you run this with different bit sizes, you'll notice different binary-sounding patterns for the number of results found:

```
f81f 1111100000011111
f99f 1111100110011111
fa5f 1111101001011111
fbdf 1111101111011111
fc3f 1111110000111111
fdbf 1111110110111111
fe7f 1111111001111111
ffff 1111111111111111
256 palindromes found with respect to 16 bits.
```

```
ffc3ff 111111111100001111111111
ffdbff 111111111101101111111111
ffe7ff 111111111110011111111111
ffffff 111111111111111111111111
4096 palindromes found with respect to 24 bits.
```

Basically, I think you could just make a program that *generates* the palindromes recursively or iteratively without even actually *iterating* over all of them.  Basically, I think I could make a python program that generates these faster than a C program could iterate over them.


## Code for The Skipper
In each of the above cases, the number of palindromes that are available is the number of possible combinations for BITS/2.  Or, 2 ^ (BITS/2).

So what we can do is take the number of bytes involved and divide by 2, so we'll iterate all the possible values for the left-side bytes.  Then we'll produce mirror images of those in the right-side bytes.

```c
void palindrome_skipper() {
    printf("Palindrome - skipper\n");
    initialize_table();

    //print_table();

    unsigned long num_palindromes = 0;
    unsigned long last_num = 1 << (BITS / 2);
    for (unsigned long num=0; num < last_num; num++) {
        unsigned char b0 = num >> 8;
        unsigned char b1 = num & 0xff;
        unsigned char b2 = byte_palindromes[b1];
        unsigned char b3 = byte_palindromes[b0];
        unsigned long pal = b3 << 24 | b2 << 16 | b1 << 8 | b0;
        print_num(pal);
        num_palindromes++;
    }
}
```

# Timing
I removed the printing of numbers and ran it for 30 bits.  I used 30 bits because it's meaty enough to give the computer something to think about, but doesn't take too long.

| Approach | Time |
|----------|------|
| Naive -O3   | 1.96s|
| Unrolled Ands -O3 | 2.12s |
| Patterns -O3 | 0.5s |
| Patterns (unoptimized) | 3.19s |
| Naive (unoptimized)   | 6.41s|
| Unrolled Ands (unoptimized) | 3.11s |
| Skipper -O3 | negligible* or 0.13s |

Note that only the Naive version is correct, and there is some kind of problem in the other versions that I didn't track down.  But the order of magnitude of the speeds is correct.

\* Negligible time for The Skipper is because when you turn on optimization the compiler is able to determine at compile time how many loop iterations there will be, and just prints that number.  If I turn on number printing, I get 0.13s, which isn't a fair comparison to the other tests because they don't print numbers.

# Conclusion
Of all the approaches, I like the naive version best.  It was the easiest to get right, is the most maintainable, and doesn't require hand-alteration of the code for varying numbers of bits and bytes.

# Next Steps
* Make a Python version that's even faster by just iterating through the exact palindromes, instead of iterating the entire space of numbers and filtering.
* Correct issues with approaches 2 and 3.