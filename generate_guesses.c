#ifndef GENERATE_GUESSES
#define GENERATE_GUESSES

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sha256.h"
#include "crack.h"
/*
Handles the generation of all password guesses, and their guessing. Behaves
slighty differently depending on how many arguments crack recieves and passes it
*/
void generate_guesses(BYTE** password, int max_guesses, const char* password_list, int passwordlen){
    int i = 0;
    BYTE* hash;
    struct word_deriv * guesses_head;

    if(password_list){
        guesses_head = download_guesses((char*)password_list, passwordlen);
    }else{
        guesses_head = download_guesses("possible_passwords.txt", passwordlen);
    }
    generate_deriv(guesses_head);
    guess(guesses_head, password, max_guesses);
    free_guesses(guesses_head);

}
/*
Reads password gueses from a given file and loads them into word_deriv structs
all other pointers point to null
*/
struct word_deriv * download_guesses(char * filename, int passwordlen){
    int j = 0;

    FILE* fp;
    fp = fopen(filename,"r");

    struct word_deriv * guesses_head;
    struct word_deriv * current_word;

    //initialise first word
    guesses_head = (struct word_deriv *) malloc(sizeof(struct word_deriv));
    assert(guesses_head);
    guesses_head->word = (BYTE*)malloc((passwordlen + 4) * sizeof(BYTE));
    assert(guesses_head->word);
    guesses_head->wordlet_head = NULL;

    int c;
    current_word = guesses_head;
    while((c = fgetc(fp))){
        if(c == -1){
            //EOF is a tab character, finalise last word.
            current_word->wordlen = j;
            current_word->next_word = (struct word_deriv *)malloc(sizeof(struct word_deriv));
            assert(current_word);
            current_word = current_word->next_word;
            current_word->next_word = NULL;
            break;
        }
        if(c == '\t'){
            //EOF is a tab character, finalise last word.
            current_word->wordlen = j;
            current_word->next_word = (struct word_deriv *)malloc(sizeof(struct word_deriv));
            assert(current_word);
            current_word = current_word->next_word;
            current_word->next_word = NULL;
            break;
        }
        //end of a given word, marked by any of three characters (dictated by
        //dictionary format)
        if(c == 10 || c == '\n' || c == ' '){
            current_word->wordlen = j;
            current_word->word[j+1] = '\0';
            j = 0;
            //point to next word
            current_word->next_word = (struct word_deriv *)malloc(sizeof(struct word_deriv));
            assert(current_word->next_word);
            //move to next blank word
            current_word = current_word->next_word;
            //make room for new password guess
            current_word->word = (BYTE*)malloc((passwordlen + 4) * sizeof(BYTE));
            assert(current_word->word);
            current_word->wordlet_head = NULL;
            current_word->wordlet_tail = NULL;
            current_word->next_word = NULL;
        }
        //adding letter to word
        else{
            // more than x characters are not useful
            if(j == passwordlen){
                continue;
            }
            current_word->word[j] = c;
            j++;
        }
    }
    // at end of file
    fclose(fp);
    return guesses_head;
}

/*
handles freeing of word_deriv structs
*/
void free_guesses(struct word_deriv * head_guess){
    struct word_deriv * current_guess;
    struct word_deriv * next_guess;
    current_guess = head_guess;
    int i = 0;
    int j = 0;
    while(current_guess->next_word != NULL){
        next_guess = current_guess->next_word;
        j += free_wordlets(current_guess->wordlet_head);
        free(current_guess);
        i++;
        current_guess = next_guess;
    }
    //free last node
    j += free_wordlets(current_guess->wordlet_head);
    free(current_guess);
    //printf("freed %d nodes\nfreed %d wordlets\n",i,j );

}

/*
handles freeing of wordlet structs, returns how many were freed (if you want
to count)
*/

int free_wordlets(struct wordlet * head_wordlet){
    struct wordlet * current_wordlet;
    struct wordlet * next_wordlet;
    current_wordlet = head_wordlet;
    int i = 0;
    if(head_wordlet == NULL){
        return 0 ;
    }
    while(current_wordlet->next_wordlet != NULL){
        next_wordlet = current_wordlet->next_wordlet;
        free(current_wordlet->word);
        free(current_wordlet);
        i++;
        current_wordlet = next_wordlet;
    }
    free(current_wordlet);
    return i;
}
/*
handles guessing of linked list of guesses, the head of which is given. Checks
their sha256 hash value against the hash values
*/
void guess(struct word_deriv * head_guess, BYTE** password, int max_guesses){
    struct word_deriv * current_guess;
    struct word_deriv * next_guess;
    struct wordlet * current_wordlet;
    struct wordlet * next_wordlet;

    FILE * fp;
    fp = fopen("name_wasnt_specified.txt", "w");

    int tot_passwds = 0;
    // find out how many passwords we have
    while(password[tot_passwds]){
        tot_passwds++;
    }

    current_guess = head_guess;
    current_wordlet = current_guess->wordlet_head;
    int i = 0;
    int guesses = 0;
    BYTE* hash;

    while(current_guess->next_word != NULL){
        //test guess
        guesses++;
        hash = sha256_process(current_guess->word);
        if(max_guesses){
            printf("%s\n",current_guess->word);
        }
        //compare current guess' hash with all hashes given
        for(i = 0; i < tot_passwds; i++){
            if(!memcmp(hash, password[i], SHA256_BLOCK_SIZE)){
                if(!max_guesses){
                    fprintf(fp,"%s %d\n", current_guess->word ,i + 1);
                    printf("%s %d\n", current_guess->word, i + 1);
                }
            }
        }
        //only for part 2
        if(max_guesses && guesses >= max_guesses){
            break;
        }
        //test guess derivatives (wordlets)
        if(current_guess->wordlet_head){
            current_wordlet = current_guess->wordlet_head;
        }
        //if no wordlets on this word, go to next one
        else{
            current_guess = current_guess->next_word;
            continue;
        }

        while(current_wordlet->next_wordlet != NULL){
            guesses++;
            if(max_guesses){
                printf("%s\n",current_wordlet->word);
            }
            hash = sha256_process(current_wordlet->word);
            //compare current wordlet's hash with all hash values given
            for(i = 0; i < tot_passwds; i++){
                if(!memcmp(hash, password[i], SHA256_BLOCK_SIZE)){
                    if(!max_guesses){
                        fprintf(fp,"%s %d\n", current_wordlet->word ,i + 1);
                        printf("%s %d\n", current_wordlet->word, i + 1);

                    }
                }
            }
            if(max_guesses && guesses >= max_guesses){
                break;
            }
            current_wordlet = current_wordlet->next_wordlet;
        }
        current_guess = current_guess->next_word;
    }
}


/*
condenses sha256 functionality we were given
*/
unsigned char* sha256_process(BYTE* text){
    int i = 0;
    for(i = 0; i< PASSWORD_LEN; i++){
        if(text[i] == '\0'){

            break;
        }
    }

    BYTE* buffer = (BYTE*) malloc(SHA256_BLOCK_SIZE);
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, text,i);
    sha256_final(&ctx, buffer);
    return buffer;
}

#endif
