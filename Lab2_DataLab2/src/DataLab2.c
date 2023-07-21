/* CSED 211 Fall '2021.  Lab L2 */
#if 0
FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict compared to the previous homework.  
You are allowed to use looping and conditional control.  
You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
1. Define or use any macros.
2. Define any additional functions in this file.
3. Call any functions.
4. Use any form of casting.
5. Use any data type other than int or unsigned.This means that you
cannot use arrays, structs, or unions.
6. Use any floating point data types, operations, or constants.

#endif

/*
 * float_neg - Return bit-level equivalent of expression -f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Rating: 2
 */
unsigned float_neg(unsigned uf) {
      unsigned sign = (uf >> 31) & 1;
      unsigned exp = (uf >> 23) & 0xff;
      unsigned frac = uf & (0x7fffff);
      int check_NaN = ((frac != 0) && (exp == 0xff));
      if (check_NaN)
      {
          return uf;
      }
      else
      {
          return (((~sign)<<31) | (exp << 23) | frac);
      }
}
/* 
 * float_i2f - Return bit-level equivalent of expression (float) x ->float로 변환한 비트값 반환
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point values.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Rating: 4
 */
unsigned float_i2f(int x) {
    //정수의 비트 표현을 float로 바꾸기

    unsigned sign = (x >> 31) & 1;
    unsigned exp;
    int E;
    unsigned frac;
    unsigned up=x & 0xff;;
    if (x == 0)
        return 0;
    if (x == (0x7fffffff+1))//INT_MIN만 따로 뺌
    {
        return 0xCF000000;//11001111000000000000000000000000
    }
    if (sign)
    {
        x = -x;
    }//음수일 경우 양수로 전환
        E = 1;
        while ((x >> E) != 0)
            E++;
        E--;
        exp = E + 127;

        x = x << (31 - E);//32개만큼 비트 생성
        frac = (x >> 8)&0x7fffff;//맨앞 1 생략

        if (E > 23)//frac파트로 다 나타낼 수 없을 때 아래 파트 반올림해주기
        {
            if (((up == 0x80) && (frac & 1)) || (up > 0x80))
            {
                frac++;
            
            if ((frac >> 23)!=0)
            {
                frac = 0;
                exp++;
            }
            }
        }
    return (sign<<31) | (exp << 23) | frac;   

}
/* 
 * float_twice - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Rating: 4
 */
unsigned float_twice(unsigned uf) {
    unsigned sign = (uf >> 31) & 1;
    unsigned exp = (uf >> 23) & 0xff;
    unsigned frac = uf & (0x7fffff);
    int check_NaN = ((frac != 0) && (exp == 0xff));
    if (check_NaN)
    {
        return uf;
    }
    else if (exp ==0)//denormalized value
    {
        frac = frac << 1;
        
        if ((frac>>23)!=0)
        {
            frac = frac & 0x7fffff;
            exp ++;
        }
       
    }
    else if(exp<0xff)//normalized인 경우
    {
        exp++;
        if (exp == 0xff)
            frac = 0;
        //infinity case

    }
    return (sign << 31) | (exp << 23) | frac;
}

/* 
 * float_abs - Return bit-level equivalent of absolute value of f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Rating: 2
 */
unsigned float_abs(unsigned uf) {
    unsigned sign = (uf >> 31) & 1;
    unsigned exp = (uf >> 23) & 0xff;
    unsigned frac = uf & (0x7fffff);
    int check_NaN = ((frac != 0) && (exp == 0xff));
    if (check_NaN)
        return uf;
    else { return 0x7fffffff & uf; }//절댓값 반환

}
 /* float_half - Return bit-level equivalent of expression 0.5*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Rating: 4
 */
unsigned float_half(unsigned uf) {
    unsigned sign = (uf >> 31) & 1;
    unsigned exp = (uf >> 23) & 0xff;
    unsigned frac = uf & (0x7fffff);
    int up = ((frac & 3) >> 1) & (uf & 1);
    int check_NaN = ((frac != 0) && (exp == 0xff));
    if (check_NaN)
        return uf;
    else if (exp == 0)
    {
        frac = frac >> 1;
        frac = frac + up;
    }
    else if (exp==1)//normal to denormal!
    {
        exp = 0;
        frac = (((uf & 0x7fffffff) >> 1) + up) & 0x7fffff;        
    }
    else {//normalized value;
        exp = exp - 1;
    }
    return(sign << 31) | (exp << 23) | frac;
}
