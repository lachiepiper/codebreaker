#ifndef CRACK
#define CRACK

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sha256.h"
#include "crack.h"


int main(int argc, char const *argv[]) {
    int i = 0;
    int j=0;
    //number of passwords made;
    int password_num = 1;
    int password_length = PASSWORD_LEN;
    FILE* fp;
    //to hold password hashes
    BYTE** password;
    //initialise aray and space for first hash
    password = (BYTE**)malloc(password_num * sizeof(BYTE*));
    assert(password);
    password[0] = (BYTE*)malloc(SHA256_BLOCK_SIZE + 4);
    assert(password[0]);

    //behave differently depending on input
    switch (argc) {
        case 1:
            password_length = 6;
            fp = fopen("pwd6sha256","r");
            //load hashes
            while (fread(password[i], SHA256_BLOCK_SIZE, 1, fp)){
                i++;
                //make more space if needed
                if(i >= password_num){
                    password_num *= 2;
                    password = (BYTE**)realloc(password, password_num * sizeof(BYTE*));
                    assert(password);
                }
                password[i] = (BYTE*)malloc(SHA256_BLOCK_SIZE + 4);
                assert(password[i]);
            }
            //set last pointer as sentinel
            password[i] = NULL;
            generate_guesses(password,0, NULL, password_length);
            break;
        case 2:
            password_length = 6;
            fp = fopen("pwd6sha256","r");
            //load hashes
            while (fread(password[i], SHA256_BLOCK_SIZE, 1, fp)){
                i++;
                //make more space if needed
                if(i >= password_num){
                    password_num *= 2;
                    password = (BYTE**)realloc(password, password_num * sizeof(BYTE*));
                    assert(password);
                }
                password[i] = (BYTE*)malloc(SHA256_BLOCK_SIZE + 4);
                assert(password[i]);
            }
            //set last pointer as sentinel
            password[i] = NULL;
            generate_guesses(password,atoi(argv[1]), NULL, password_length);
            break;
        case 3:
            fp = fopen(argv[2],"r");
            while (fread(password[i], SHA256_BLOCK_SIZE, 1, fp)){
                i++;
                //make more space if needed
                if(i >= password_num){
                    password_num *= 2;
                    password = (BYTE**)realloc(password, password_num * sizeof(BYTE*));
                    assert(password);
                }
                password[i] = (BYTE*)malloc(SHA256_BLOCK_SIZE + 4);
                assert(password[i]);
            }
            //set last pointer as sentinel
            password[i] = NULL;
            generate_guesses(password,0,argv[1], password_length);
            break;
        default: printf("wrong number of arguments\n");
    }

    for(j = 0; j< i; j++){
        free(password[j]);
    }
    free(password);
    return 0;
}

#endif
