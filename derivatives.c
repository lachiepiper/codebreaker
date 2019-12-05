#ifndef DERIVATIVES
#define DERIVATIVES

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sha256.h"
#include "crack.h"


/*
handles the creatinon of derivatives of a certain guess word
*/
void generate_deriv(struct word_deriv * head_guess){
    struct word_deriv * current_guess;
    current_guess = head_guess;
    while(current_guess->next_word != NULL){
        variants(current_guess);
        insert_capitals(current_guess);
        //only pad numbers for small passwords
        if(current_guess->wordlen < 6){
            pad_numbers(current_guess);
        }
        current_guess = current_guess->next_word;
    }
}
#endif
