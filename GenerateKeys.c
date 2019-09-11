//
//  GenerateKeys.c
//  CS378PA
//
//  Created by Alex Wyllie on 3/27/18.
//  Copyright © 2018 Alex Wyllie. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <gmp.h>


//int mpz_probab_prime_p (const mpz t n, int reps) uses Miller-Rabine test (after some trial divisions)
typedef enum { false, true } bool;

bool millerRabin(mpz_t num);
void modExp(mpz_t returned, mpz_t base, mpz_t power, mpz_t modulo);
void euclidsAlgorithm(mpz_t returned, mpz_t num1, mpz_t num2);
void extendedEuclids(mpz_t returned, mpz_t num, mpz_t modulo);

int millerRabinTries = 0;

#define primeCheckNum 50

int main(int argc, const char * argv[]) {
    
    if (argc != 2) {
        printf("GenerateKeys expects 1 argument. \nUsage:\n./GenerateKeys <primesLengthInBits>\n");
        return 1;
    }
    
    unsigned long primeLength = 0;
    primeLength = atoi(argv[1]);
    if (primeLength < 335) {
        printf("GenerateKeys expects an argument larger than 335.\nUsage:\n./GenerateKeys <priesLengthInBits>\n");
        return 1;
    }
    gmp_randstate_t state;
    gmp_randinit_default(state); //Generate a state for random number generator.
    mpz_t p;
    mpz_init2(p, primeLength);
    while (!millerRabin(p)) {
        mpz_urandomb(p, state, primeLength);
    }
    mpz_t q;
    mpz_init2(q, primeLength);
    while (!millerRabin(q)) {
        mpz_urandomb(q, state, primeLength);
    }
    mpz_t n;
    mpz_init(n);
    mpz_mul(n, p, q);
    mpz_sub_ui(p, p, 1);
    mpz_sub_ui(q, q, 1);
    mpz_t pq; //Will actually be (p-1)(q-1), but I am bad at naming.
    mpz_init(pq);
    mpz_mul(pq, p, q);
    mpz_t e;
    mpz_init(e);
    mpz_set_ui(e, 65537);
    mpz_t d;
    mpz_init(d);
    extendedEuclids(d, e, pq);
    while (mpz_cmp_ui(d, 0) == 0) {
        mpz_urandomb(e, state, 20);
        extendedEuclids(d, e, pq);
    }
    
    //Output keys to files.
    FILE *privatekey = fopen("/private_key", "w+");
    FILE *publicKey = fopen("/public_key", "w+");
    if (privatekey == NULL || publicKey == NULL) {
        printf("Error opening files\n");
        printf("%d", errno);
        return 1;
    }
    printf("Check 1\n");
    if (mpz_out_str(privatekey, 16, d) == 0) {
        printf("Error writing keys to file.\n");
        return 1;
    }
    printf("Check 2\n");
    fprintf(privatekey, "\n");
    printf("Check 3\n");
    if (mpz_out_str(privatekey, 16, n) == 0) {
        printf("Error writing keys to file.\n");
        return 1;
    }
    if (mpz_out_str(publicKey, 16, e) == 0) {
        printf("Error writing keys to file.\n");
        return 1;
    }
    fprintf(publicKey, "\n");
    if (mpz_out_str(publicKey, 16, n) == 0) {
        printf("Error writing keys to file.\n");
        return 1;
    }
    printf("Public key is now stored in /public.key\nPrivate key is now stored in /private.key\nDo not share the private key\n");
    return 0;
}

bool millerRabin(mpz_t num) {
    //Cover the most common prime factors for some linear speedup of algorithm.
    if (mpz_divisible_ui_p(num, 2)) {
        return false;
    }
    if (mpz_divisible_ui_p(num, 3)) {
        return false;
    }
    if (mpz_divisible_ui_p(num, 5)) {
        return false;
    }
    if (mpz_divisible_ui_p(num, 7)) {
        return false;
    }
    //Miller-rabin test
    mpz_t numMinusOne;
    mpz_init(numMinusOne);
    mpz_set(numMinusOne, num);
    mpz_sub_ui(numMinusOne, num, 1);
    mpz_t m;
    mpz_init(m);
    mpz_set(m, num);
    mpz_sub_ui(m, m, 1);
    mpz_t r;
    mpz_init(r);
    mpz_t two;
    mpz_init(two);
    mpz_set_ui(two, 2);
    while (!mpz_congruent_ui_p(m, 2, 1)) {
        mpz_divexact_ui(m, m, 2);
        mpz_add_ui(r, r, 1);
    }
    gmp_randstate_t state;
    gmp_randinit_default(state);
    int i = 0;
continueLoop: for (; i < primeCheckNum; i++) {
        mpz_t k;
        mpz_init(k);
        mpz_t numMinusFour;
        mpz_init(numMinusFour);
        mpz_sub_ui(numMinusFour, num, 4);
        mpz_urandomm(k, state, numMinusFour);
        mpz_add_ui(k, k, 2);
        mpz_t x;
        mpz_init(x);
        modExp(x, k, m, num);
        if (mpz_cmp(x, numMinusOne) == 0 || mpz_cmp_ui(x, 1) == 0) {
            i++;
            goto continueLoop;
        }
        mpz_t j;
        mpz_init(j);
        while (mpz_cmp(j, r) < 0) {
            modExp(x, x, two, num);
            if (mpz_cmp_ui(x, 1) == 0) {
                return false;
            }
            if (mpz_cmp(x, numMinusOne) == 0) {
                i++;
                goto continueLoop;
            }
            mpz_add_ui(j, j, 1);
        }
        return false;
    }
    return true;
}

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
    while (mpz_cmp_ui(power, 0) > 0) {
        if (mpz_fdiv_ui(power, 2) == 1) {
            mpz_mul(result, result, base);
            mpz_mod(result, result, modulo);
        }
        mpz_tdiv_q_2exp(power, power, 1);
        mpz_mul(base, base, base);
        mpz_mod(base, base, modulo);
    }
    mpz_set(returned, result);
}

void euclidsAlgorithm(mpz_t returned, mpz_t num1, mpz_t num2) {
    mpz_t temp;
    mpz_init(temp);
    while (mpz_cmp_ui(num2, 0) != 0) {
        mpz_set(temp, num2);
        mpz_mod(num2, num1, num2);
        mpz_set(num1, temp);
    }
    mpz_set(returned, num1);
}

void extendedEuclids(mpz_t returned, mpz_t num, mpz_t modulo) {
    mpz_t t;
    mpz_init(t);
    mpz_t newt;
    mpz_init_set_ui(newt, 1);
    mpz_t r;
    mpz_init(r);
    mpz_set(r, modulo);
    mpz_t newr;
    mpz_init(newr);
    mpz_set(newr, num);
    mpz_t quotient;
    mpz_init(quotient);
    mpz_t temp;
    mpz_init(temp);
    while (mpz_cmp_ui(newr, 0) != 0) {
        mpz_fdiv_q(quotient, r, newr);
        mpz_mul(temp, quotient, newt);
        mpz_sub(temp, t, temp);
        mpz_set(t, newt);
        mpz_set(newt, temp);
        mpz_mul(temp, quotient, newr);
        mpz_sub(temp, r, temp);
        mpz_set(r, newt);
        mpz_set(newr, temp);
    }
    if (mpz_cmp_ui(r, 1) > 0) {
        mpz_set_ui(returned, 0);
    }
    if (mpz_cmp_ui(t, 0) < 0) {
        mpz_add(t, t, modulo);
    }
    mpz_set(returned, t);
}
