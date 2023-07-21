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
 * float_i2f - Return bit-level equivalent of expression (float) x ->float�� ��ȯ�� ��Ʈ�� ��ȯ
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point values.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Rating: 4
 */
unsigned float_i2f(int x) {
    //������ ��Ʈ ǥ���� float�� �ٲٱ�

    unsigned sign = (x >> 31) & 1;
    unsigned exp;
    int E;
    unsigned frac;
    unsigned up=x & 0xff;;
    if (x == 0)
        return 0;
    if (x == (0x7fffffff+1))//INT_MIN�� ���� ��
    {
        return 0xCF000000;//11001111000000000000000000000000
    }
    if (sign)
    {
        x = -x;
    }//������ ��� ����� ��ȯ
        E = 1;
        while ((x >> E) != 0)
            E++;
        E--;
        exp = E + 127;

        x = x << (31 - E);//32����ŭ ��Ʈ ����
        frac = (x >> 8)&0x7fffff;//�Ǿ� 1 ����

        if (E > 23)//frac��Ʈ�� �� ��Ÿ�� �� ���� �� �Ʒ� ��Ʈ �ݿø����ֱ�
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
    else if(exp<0xff)//normalized�� ���
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
    else { return 0x7fffffff & uf; }//���� ��ȯ

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
