#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BITS 30 // Number of bits we'll check palindromes for.
#define FALSE 0
#define TRUE -1

void usage() {
    printf("Usage: palindrome {approach}\n");
    printf("Where {approach} is:\n");
    printf("  1 Naive\n");
}

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


int is_palindrome_ands8(unsigned long num) {
    // determine if num is a palindrome.
    return (
        (((num & 0x80) != 0) == ((num & 0x01) != 0)) &&
        (((num & 0x40) != 0) == ((num & 0x02) != 0)) &&
        (((num & 0x20) != 0) == ((num & 0x04) != 0)) &&
        (((num & 0x10) != 0) == ((num & 0x08) != 0))
    );
}

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

unsigned char byte_palindromes[256];


unsigned char reverse_byte(unsigned char byte) {
    unsigned char mask = 0x80;
    unsigned char result = 0;

    for (int bit = 0; bit < 8; bit++) {
        unsigned char mask = 1 << bit;
        if (byte & mask) {
            result |= 1 << (7 - bit);
        }
    }
    return result;
}


void initialize_table() {
    for (unsigned int i = 0; i < 256; i++) {
        byte_palindromes[i] = reverse_byte((unsigned char)i);
    }  
}

void print_table() {
    for (int i=0; i < 256; i++) {
        printf("%x %x\n", i, byte_palindromes[i]);
    }
}

int is_palindrome_pattern16(unsigned long num) {
    unsigned char b1 = num >> 8;
    unsigned char b0 = num & 0xff;
    return b1 == byte_palindromes[b0];
}

int is_palindrome_pattern32(unsigned long num) {
    unsigned char b3 = (num >> 24);
    unsigned char b2 = (num >> 16) & 0xff;
    unsigned char b1 = (num >> 8) & 0xff;
    unsigned char b0 = num & 0xff;
    return b0 == byte_palindromes[b3] &&
        b1 == byte_palindromes[b2];
}

void print_bits(unsigned long num) {
    unsigned long mask = 1 << (BITS - 1);
    while (mask) {
        char bit = num & mask ? '1' : '0';
        putchar(bit);
        mask >>= 1;
    }
}

// print_num prints a number with its binary representation.
void print_num(unsigned long num) {
    printf("%lx ", num);
    print_bits(num);
    putchar('\n');
}

void palindrome_naive() {
    printf("Palindrome - naive\n");

    unsigned long num_palindromes = 0;
    unsigned long last_num = 1 << BITS;
    for (unsigned long num=0; num < last_num; num++) {
        if (!is_palindrome_naive(num))
            continue;
        //print_num(num);
        num_palindromes++;
    }
    printf("%lu palindromes found with respect to %d bits.\n", num_palindromes, BITS);
}

void palindrome_ands() {
    printf("Palindrome - unrolled ands\n");
    unsigned long num_palindromes = 0;
    unsigned long last_num = 1 << BITS;
    for (unsigned long num=0; num < last_num; num++) {
        if (!is_palindrome_ands16(num))
            continue;
        //print_num(num);
        num_palindromes++;
    }
    printf("%lu palindromes found with respect to %d bits.\n", num_palindromes, BITS);
}

void palindrome_pattern() {
    printf("Palindrome - pattern\n");
    initialize_table();
    //print_table();

    unsigned long num_palindromes = 0;
    unsigned long last_num = 1 << BITS;
    for (unsigned long num=0; num < last_num; num++) {
        if (!is_palindrome_pattern32(num))
            continue;
        //print_num(num);
        num_palindromes++;
    }
    printf("%lu palindromes found with respect to %d bits.\n", num_palindromes, BITS);
}

int main(int argc, char* argv[]) {
    printf("Binary Palindromes\n");
    if (argc != 2) {
        usage();
        exit(1);
    }

    if (strlen(argv[1]) != 1) {
        usage();
        exit(1);
    }

    char approach = argv[1][0];
    switch (approach) {
        case '1':
            palindrome_naive();
            break;
        case '2':
            palindrome_ands();
            break;
        case '3':
            palindrome_pattern();
            break;

        default:
            usage();
            exit(1);
    }
    return 0;
}
