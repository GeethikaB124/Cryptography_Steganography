#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include "keyExpansion.h"
#include <iostream>
#include "lookupEncrypt.h"

/* The round key is Xored with the state matrix
*/
void addRoundKey(unsigned char * stateArray, unsigned char * expandedKey, int start){
    for(int i = 0; i < 16; i++ ){
        stateArray [i] = stateArray [i] ^ expandedKey [i + start];
    }
}

/*The bytes in the state matrix 
are replaced using the s box
*/
void subBytes(unsigned char * stateArray){
    for(int i = 0; i < 16; i++){
        stateArray [i] = sbox [stateArray [i]];
    }
}


/*In the 4x4 state matrix: 
    -The second row is shifted by 1 to the left
    -The third row is shifted by 2 to left
    -The fourth row is shifted by 3 to the left
*/
void shiftRows(unsigned char * stateArray){
    unsigned char temp [16];

    //1st column
    temp [0] = stateArray [0];
    temp [1] = stateArray [5];
    temp [2] = stateArray [10];
    temp [3] = stateArray [15]; 

    //2nd column
    temp [4] = stateArray [4];
    temp [5] = stateArray [9];
    temp [6] = stateArray [14];
    temp [7] = stateArray [3];

    //3rd column
    temp [8] = stateArray [8];
    temp [9] = stateArray [13];
    temp [10] = stateArray [2];
    temp [11] = stateArray [7];

    //4th column
    temp [12] = stateArray [12];
    temp [13] = stateArray [1];
    temp [14] = stateArray [6];
    temp [15] = stateArray [11];

    for (int i = 0; i < 16; i++){
        stateArray [i] = temp [i];
    }
}

//The columns are multiplied by a constant matrix
void mixColumns(unsigned char * stateArray){
    unsigned char temp[16];

    for (int i = 0; i < 4; i ++){
        for (int j = 0; j < 4; j ++){
            if ( i == 0){
                temp[j*4] = (unsigned char) mul2[stateArray[4*j]] ^ mul3[stateArray[4*j + 1]] ^ stateArray[4*j + 2] ^ stateArray[4*j + 3];
            }
            else if (i == 1){
                temp [1 + 4*j] = (unsigned char) stateArray [4*j] ^ mul2[stateArray [4*j + 1]] ^ mul3[stateArray[4*j + 2]] ^ stateArray[4*j + 3];
            }
            else if (i == 2){
                temp [2 + 4*j] = (unsigned char) stateArray[4*j] ^ stateArray[4*j + 1] ^ mul2[stateArray[4*j + 2]] ^ mul3[stateArray[4*j + 3]];
            }
            else if(i == 3){
                temp [3 + 4*j] = (unsigned char) mul3[stateArray[4*j]] ^ stateArray[4*j + 1] ^ stateArray[4*j + 2] ^ mul2[stateArray[4*j + 3]];
            }
        }
    }
	for (int i = 0; i < 16; i++) {
		stateArray[i] = temp[i];
	}
}

//encryption function
void encrypt(unsigned char * cipherText, unsigned char * expandedKey){

    //Round 0:
    addRoundKey(cipherText, expandedKey, 0);

    for (int round = 1; round <= 9; round ++){
        subBytes(cipherText);
       
        shiftRows(cipherText);
       
        mixColumns(cipherText);

        addRoundKey(cipherText, expandedKey, (16*round));
    }

    //Round 10 (doesnt have mixColumns):
    subBytes(cipherText);
    shiftRows(cipherText);
    addRoundKey(cipherText, expandedKey, 160);

}

#endif