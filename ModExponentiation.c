//
//  ModExponentiation.c
//  CS378PA
//
//  Created by Alex Wyllie on 3/28/18.
//  Copyright © 2018 Alex Wyllie. All rights reserved.
//

#include "ModExponentiation.h"

void modExp(mpz_t returned, mpz_t base, mpz_t power, mpz_t modulo) {
    mpz_t one;
    mpz_init_set_ui(one, 1);
    mpz_t result;
    mpz_init(result);
    if (mpz_cmp(modulo, one) == 0) {
        mpz_set(returned, result);
    }
    mpz_t indexBit;
    mpz_init(indexBit);
    mpz_set_ui(result, 1);
    mpz_mod(base, base, modulo);
    while (mpz_cmp_ui(power, 1) > 0) {
        if (mpz_fdiv_ui(power, 2) == 1) {
            mpz_mul(result, result, base);
            mpz_mod(result, result, modulo);
        }
        mpz_tdiv_q_2exp(exponent, exponent, 1);
        mpz_mul(base, base, base);
        mpz_mod(base, base, modulo);
    }
    mpz_set(returned, result);
}
