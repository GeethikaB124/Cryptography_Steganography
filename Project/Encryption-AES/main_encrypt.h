#include <iostream>
#include <sstream>
#include "keyExpansion.h"
#include "encryption.h"
#include <fstream>
#include <cryptopp/eccrypto.h>
#include <cryptopp/osrng.h>
#include <cryptopp/hex.h>
#include <cryptopp/oids.h>
#include <cryptopp/files.h>

using namespace std;
using namespace CryptoPP;

void encrypt(const string& key) {
    AutoSeededRandomPool rng;
    string ciphertext;

    // Provided public key
    string publicKeyStr = "3059301306072A8648CE3D020106082A8648CE3D030107034200041FF2F3F83208A776D6786818B1C8914F39C205B29A3A5EF815315B7371D54D4966AD065BDDD21948A57FC43F7E2A5843F93ED230012A42599C8D843C70E80DC6";
    ECIES<ECP>::PublicKey publicKey;
    StringSource ssPublicKey(publicKeyStr, true, new HexDecoder);
    publicKey.Load(ssPublicKey);

    // Encrypt the key using the provided public key
    ECIES<ECP>::Encryptor encryptor(publicKey);
    StringSource(key, true, new PK_EncryptorFilter(rng, encryptor, new StringSink(ciphertext)));
    ofstream file("../Decryption-AES/key.txt");
    if (file.is_open()) {
        file << ciphertext;
        file.close();
        cout << "Random key generated and stored in Key.txt." << endl;
    } else {
        cerr << "Error: Unable to open the file." << std::endl;
    }
}


string generateRandomKey(int length) {
    const string charset = "0123456789";
    std::string key;
    srand(static_cast<unsigned int>(time(0)));

    for (int i = 0; i < length; ++i) {
        key += charset[rand() % charset.length()];
    }

    return key;
}

string generateAndStoreRandomKey(const std::string& fileName, int keyLength) {
    string randomKey = generateRandomKey(keyLength);
    cout<<randomKey<<endl;
    encrypt(randomKey);
    return randomKey;
}
//to convert string to hex
void convertStr_Hex(string ascii_str, char *hex_str){
    int loop = 0;
    int i = 0;
    for (int i = 0; i < 16; i ++){
        sprintf((char *)(hex_str+loop),"%02X", ascii_str[i]);
        loop += 2;
        if(i == 15) continue;
        sprintf((char *)(hex_str + loop), " ");
        loop ++;
        
    }
}

string convertInt_Bin(int n){
    string r, temp = "";
    int x;
    while (n != 0){
        r = (n %2 == 0 ? "0":"1") + r;
        n = n / 2;
    }
    if (r.length() != 8){
        x = 8 - r.length();
        for (int i = 0; i < x; i++){
            temp = temp + "0";
        }
    }
    return temp + r;
}


void encrypt_final() {
    string plaintext, key_input;

    cout << "Please enter the text to be encrypted: ";
    getline(cin, plaintext);
    
    //padding the plaintext:
    int plaintext_len = plaintext.length();
    int plaintext_padded_len;

    //creating length of padded plaintext
    if (plaintext_len % 16 != 0){
        plaintext_padded_len = (16 - (plaintext_len % 16)) + plaintext_len;
    }
    else{
        plaintext_padded_len = plaintext_len;
    }

    //allocating memory on the heap for cipherText
    unsigned char * cipherText = new unsigned char[plaintext_padded_len];

    //creating cipherText with padding
    for (int i = 0; i < plaintext_padded_len; i ++){
        if (i >= plaintext_len){
            cipherText [i] = 0;
        }
        else{
            cipherText[i] = plaintext [i];
        }
    }
    key_input = generateAndStoreRandomKey("key.txt", 16);
    int len = key_input.length();

    char hex_str [3*len - 1];
    convertStr_Hex(key_input, hex_str);    

    //creating the key.
    istringstream key_stream(hex_str);
    unsigned char key [16];
    unsigned int x;

    // cout << "Key: ";
    for(int i = 0; i < 16; i++){
	    key_stream >> hex >> x;
		key [i] = x; 
        // cout << hex << (int) key [i] << " ";
	}
    cout << endl;

    //creating 11 keys for the 11 rounds
    unsigned char expandedKey [176];
    key_expansion(key, expandedKey);

    //encryption
    for(int i = 0; i < plaintext_padded_len; i += 16 ){
        encrypt(cipherText + i, expandedKey);
    }

    ofstream myfile ("../Kmeans and LSB encryption/example.txt");
    if (myfile.is_open())
    {
        for(int i = 0; i < plaintext_padded_len; i ++){
            myfile << convertInt_Bin((int) cipherText [i]) ;
        }
        myfile.close();
    }
    else cout << "Unable to open file";
}
