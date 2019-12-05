#ifndef INSERT
#define INSERT

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "sha256.h"
#include "crack.h"

/*
if a word is less than 6 characters, the extra space is padded with all combinations
of numbers. each different conbination is addded as a wordlet on to original word.
It is very common for people to put a 2 or 3 digit ending to their passwords,
representing a lucky number or a year born.
*/
void pad_numbers(struct word_deriv * guess){
    int i = 0;
    int j = 0;
    int len = guess->wordlen;
    char varied_word[6];
    char numstr[6 - len + 2];
    int digit_length = 0;

    //copy word
    for(i = 0; i < len; i++){
        varied_word[i] = guess->word[i];
    }
    i = 0;

    while(digit_length <= (6 - len)){
        //convert int to string
        snprintf(numstr,(6 - len + 1), "%d", i);
        //append number padding to word
        while(numstr[j] != '\0'){
            varied_word[len + j] = numstr[j];
            j++;
        }
        j = 0;
        addWordlet(guess, (BYTE*)varied_word , sizeof(varied_word) / sizeof(char));
        i++;
        //make sure digit has the right length
        digit_length = floor(log10(abs(i))) + 1;
    }
}
/*
replaces lower case letters with upper case ones.
each different conbination of letters is addded as a wordlet on to original word.
It is common for people to put capital letters randomly assorted into their
passwords.
*/
void insert_capitals(struct word_deriv * guess){
    int i = 0;
    int len = guess->wordlen;
    BYTE varied_word[len];
    int change_letter[len];

    for(i = 0; i < len; i++){
        //copy word
        varied_word[i] = guess->word[i];
        //record changable letter
        if(guess->word[i] <= 122 && guess->word[i] >= 97 ){
            change_letter[i] = 1;
        }
        else{
            change_letter[i] = 0;
        }
    }

    for(i = 0; i < len; i++){
        if(change_letter[i]){
            //change to capital
            varied_word[i] -= 32;
            addWordlet(guess, varied_word, len);
            //change back for next word
            varied_word[i] += 32;
        }
    }
}
/*
Replaces specific characters with different characters that resemble them.
This is a common strategy when creating a safe passord
*/
void variants(struct word_deriv * guess){
    int i = 0;
    int j = 0;
    int len = guess->wordlen;
    int changes = 0;
    BYTE varied_word[len +1];
    int change_letter[len+ 1];

    for(i = 0; i < len; i++){
        //copy word
        varied_word[i] = guess->word[i];
        //record changable letter
        if(strchr("lioeajg\0",guess->word[i])){
            change_letter[i] = 1;
            changes++;
        }
        else{
            change_letter[i] = 0;
        }
    }
    for(i = 0; i < len; i++){
        if(change_letter[i]){
            switch (guess->word[i]) {
                case 'o' :
                    varied_word[i] = '0';
                    addWordlet(guess, varied_word, len);
                    varied_word[i] = '*';
                    addWordlet(guess, varied_word, len);
                    varied_word[i] = guess->word[i];
                    break;
                case 'i':
                    varied_word[i] = '1';
                    addWordlet(guess, varied_word, len);
                    varied_word[i] = guess->word[i];
                    break;
                case 'e':
                    varied_word[i] = '3';
                    addWordlet(guess, varied_word, len);
                    varied_word[i] = '&';
                    addWordlet(guess, varied_word, len);
                    varied_word[i] = guess->word[i];
                    break;
                case 'a':
                    varied_word[i] = '4';
                    addWordlet(guess, varied_word, len);
                    varied_word[i] = guess->word[i];
                    break;
                case 'l':
                    varied_word[i] = '7';
                    addWordlet(guess, varied_word, len);
                    varied_word[i] = guess->word[i];
                    break;
                case 'j':
                    varied_word[i] = ']';
                    addWordlet(guess, varied_word, len);
                    varied_word[i] = guess->word[i];
                    break;
                case 'g':
                    varied_word[i] = '9';
                    addWordlet(guess, varied_word, len);
                    varied_word[i] = guess->word[i];
                    break;
            }
        }
    }
    return;
}

/*
Creates a wordlet struct and attaches it to its root word or the last worlet
in the linked list
*/
void addWordlet(struct word_deriv * guess, BYTE* varied_word, int var_word_len){
    int i = 0;
    struct wordlet * curr_wordlet;

    if(!(guess->wordlet_head)){
        //first wordlet
        guess->wordlet_head = (struct wordlet *)malloc(sizeof(struct wordlet));
        assert(guess->wordlet_head);
        guess->wordlet_tail = (struct wordlet *)malloc(sizeof(struct wordlet));
        assert(guess->wordlet_tail);
        guess->wordlet_tail = guess->wordlet_head;
        curr_wordlet = guess->wordlet_head;
        //initialise wordlet word (not necessarily the max pasword length)
        curr_wordlet->word = (BYTE*)malloc((var_word_len + 1) * sizeof(BYTE));
        assert(curr_wordlet->word);

    }
    else{
        //go to end of list
        curr_wordlet = guess->wordlet_tail;

        //make space for new wordlet and move to it
        curr_wordlet->next_wordlet = (struct wordlet *)malloc(sizeof(struct wordlet));
        assert(curr_wordlet->next_wordlet);
        curr_wordlet = curr_wordlet->next_wordlet;
        curr_wordlet->next_wordlet = NULL;
        curr_wordlet->word = (BYTE*)malloc((var_word_len + 1) * sizeof(BYTE));
        assert(curr_wordlet->word);
        //update end of tail

        guess->wordlet_tail = curr_wordlet;
    }
    //copy word in
    for(i=0;i < var_word_len; i++){
        curr_wordlet->word[i] = varied_word[i];
    }
    i++;
    curr_wordlet->word[i] = '\0';

}

#endif
