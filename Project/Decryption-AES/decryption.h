#ifndef DECRYPTION_H
#define DECRYPTION_H

#include "keyExpansion.h"
#include "lookupDecrypt.h"
#include <iostream>

/* The round key is Xored with the state matrix
*/
void addRoundKey(unsigned char * stateArray, unsigned char * expandedKey, int start){
    for(int i = 0; i < 16; i++ ){
        stateArray [i] = stateArray [i] ^ expandedKey [i + start];
    }
}

/* Same as mixColumns but the constant matrix multiplied is different */
void invMixColumns(unsigned char * stateArray) {
	unsigned char temp[16];
    
    for (int i = 0; i < 4; i ++){
        for (int j = 0; j < 4; j ++){
            if ( i == 0){
                temp[j*4] = (unsigned char)mul14[stateArray[4*j]] ^ mul11[stateArray[4*j + 1]] ^ mul13[stateArray[4*j + 2]] ^ mul9[stateArray[4*j + 3]];
            }
            else if (i == 1){
                temp [1 + 4*j] = (unsigned char)mul9[stateArray[4*j]] ^ mul14[stateArray[4*j + 1]] ^ mul11[stateArray[4*j + 2]] ^ mul13[stateArray[4*j + 3]];
            }
            else if (i == 2){
                temp [2 + 4*j] = (unsigned char)mul13[stateArray[4*j]] ^ mul9[stateArray[4*j + 1]] ^ mul14[stateArray[4*j + 2]] ^ mul11[stateArray[4*j + 3]];
            }
            else if(i == 3){
                temp [3 + 4*j] = (unsigned char)mul11[stateArray[4*j]] ^ mul13[stateArray[4*j + 1]] ^ mul9[stateArray[4*j + 2]] ^ mul14[stateArray[4*j + 3]];
            }
        }
    }
	for (int i = 0; i < 16; i++) {
		stateArray[i] = temp[i];
	}
}

/*Shifts rows right (rather than left) for decryption
1st row is left unchanged
2nd row is shifted one to the right
3rd row is shifted two to the right
4th row is shifted three to the right
*/
void shiftRows(unsigned char * stateArray) {
	unsigned char temp[16];

	// Column 1
	temp[0] = stateArray[0];
	temp[1] = stateArray[13];
	temp[2] = stateArray[10];
	temp[3] = stateArray[7];

	// Column 2
	temp[4] = stateArray[4];
	temp[5] = stateArray[1];
	temp[6] = stateArray[14];
	temp[7] = stateArray[11];

	// Column 3
	temp[8] = stateArray[8];
	temp[9] = stateArray[5];
	temp[10] = stateArray[2];
	temp[11] = stateArray[15];

	// Column 4 
	temp[12] = stateArray[12];
	temp[13] = stateArray[9];
	temp[14] = stateArray[6];
	temp[15] = stateArray[3];

	for (int i = 0; i < 16; i++) {
		stateArray[i] = temp[i];
	}
}

/*The bytes in the state matrix 
are replaced using the inverse s box
*/
void invSubBytes(unsigned char * stateArray){
    for(int i = 0; i < 16; i++){
        stateArray [i] = invsbox [stateArray [i]];
    }
}

//decryption function
void decrypt (unsigned char * decryptedText, unsigned char * expandedKey){

    //Initial round doesnt have invMixColumns
    addRoundKey (decryptedText, expandedKey, 160);

    shiftRows (decryptedText);
    
    invSubBytes (decryptedText);

    //Round 9 to Round 1
    for (int round = 9; round > 0; round --){
        
        addRoundKey (decryptedText, expandedKey, 16 * round);

        invMixColumns(decryptedText);

        shiftRows (decryptedText);

        invSubBytes (decryptedText);

    }

    //finally XOR with the first key
    addRoundKey (decryptedText, expandedKey, 0);

}

#endif