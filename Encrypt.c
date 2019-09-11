//
//  Encrypt.c
//  CS378PA
//
//  Created by Alex Wyllie on 3/27/18.
//  Copyright © 2018 Alex Wyllie. All rights reserved.
//

#include "ModExponentiation.h"


int main(int argc, const char * argv[]) { //expects arguments pointing to file containing message, file containing public key
    if (argc != 3) {
        printf("Encrypt expects 2 arguments.\nUsage:\n./Encrypt <messageFileName> <publicKeyFileName>");
        return 1;
    }
    
}
