/* CSED 211 Fall '2021.  Lab L1 */



//#if 0
//LAB L1 INSTRUCTIONS :

//#endif

/*
 * bitAnd - x&y using only ~ and |
 *   Example: bitAnd(6, 5) = 4
 *   Legal ops: ~ |
 */

int bitAnd(int x, int y)
{
    return (~(~x | ~y));
}

/*
 * addOK - Determine if can compute x+y without overflow
 *   Example: addOK(0x80000000,0x80000000) = 0,
 *            addOK(0x80000000,0x70000000) = 1,
 *   Legal ops: ! ~ & ^ | + << >>
 */

int addOK(int x, int y)
{
    return (((x^y)>>31)&1) | (~((x^(x+y))>>31) & 1);
}

/*
 * isNegative - return 1 if x < 0, return 0 otherwise
 *   Example: isNegative(-1) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */

int isNegative(int x)
{
    return ((x >> 31)&1);
}

/*
 * logicalShift - logical right shift of x by y bits, 1 <= y <= 31
 *   Example: logicalShift(-1, 1) = TMax.
 *   Legal ops: ! ~ & ^ | + << >>
 */

int logicalShift(int x, int y)
{

    return (x >> y) & ~(((0x1 << 31) >> y)<< 1);
}

/*
 * bitCount - returns count of number of 1's in word
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
 *   Legal ops: ! ~ & ^ | + << >>
 */
int bitCount(int x)
{
    int a = (0x1 | (0x1 << 8));
    a= a|(a<<16);
   
   int result = (a & x) + (a & (x >> 1)) + (a & (x >> 2)) + (a & (x >> 3)) + (a & (x >> 4)) + (a & (x >> 5)) + (a & (x >> 6)) + (a & (x >> 7));
    result += result >> 16;
    result += result >> 8;

    result &= 0xff;
    return result;

}