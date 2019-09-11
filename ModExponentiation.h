//
//  ModExponentiation.h
//  CS378PA
//
//  Created by Alex Wyllie on 3/28/18.
//  Copyright © 2018 Alex Wyllie. All rights reserved.
//

#ifndef ModExponentiation_h
#define ModExponentiation_h

#include <stdio.h>
#include <gmp.h>

void modExp(mpz_t returned, mpz_t base, mpz_t power, mpz_t modulo);

#endif /* ModExponentiation_h */
