#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/err.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

void handleErrors(void);
int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext);
int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext);



int main (void)
{
    char givenCipher[64] = "6b642b4d232d28fb9272d3aae053d6410ef9dfb267bbb9d9adcfee0f2d823f14";

    char str[17];
    int numChar;
   
    bool found = false;
    
    FILE *fp;
    
    char* wordsFile = "/home/user/Documents/KeyFinderGitHub/words.txt";

    fp = fopen(wordsFile, "r");
    
    if (fp == NULL)
    {
        printf("Could not open file %s", wordsFile);
        return 1;
    }

    while ((fgets(str, 17, fp) != NULL))
    {

        numChar = strlen(str) - 1;
	
        for (int i = numChar; i < 16; i++)
        {
            str[i] = '#';
        }
     
	unsigned char *key = (unsigned char *) str;

        unsigned char iv[16] = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11};

	unsigned char *plaintext =
		(unsigned char *)"This is lab1 in CS3840.";

	/*
	 * Buffer for ciphertext. Ensure the buffer is long enough for the
	 * ciphertext which may be longer than the plaintext, depending on the
	 * algorithm and mode.
	*/
	unsigned char ciphertext[128];

	int ciphertext_len;

	/* Encrypt the plaintext */
	ciphertext_len = encrypt (plaintext, strlen ((char *)plaintext), key, iv,
		                      ciphertext);

	char* buf_str = (char*) malloc(2*ciphertext_len+1);
	char* buf_ptr = buf_str;

	for(int i = 0; i < ciphertext_len; i++)
	{
	    buf_ptr += sprintf(buf_ptr, "%02x", ciphertext[i]);
	}
	*(buf_ptr + 1) = '\0';

        if (strncmp(givenCipher, buf_str, 32) == 0)
	{
             printf("The key is is:\n");
             printf("%s \n", str);
	     break;
	}
    }

    fclose(fp);
    return 0;
}


void handleErrors(void)
{
    ERR_print_errors_fp(stderr);
    abort();
}

int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext)
{
    EVP_CIPHER_CTX *ctx;

    int len;

    int ciphertext_len;

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    /*
     * Initialise the encryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv))
        handleErrors();

    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        handleErrors();
    ciphertext_len = len;

    /*
     * Finalise the encryption. Further ciphertext bytes may be written at
     * this stage.
     */
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        handleErrors();
    ciphertext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}


int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext)
{
    EVP_CIPHER_CTX *ctx;

    int len;

    int plaintext_len;

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    /*
     * Initialise the decryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv))
        handleErrors();

    /*
     * Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_DecryptUpdate can be called multiple times if necessary.
     */
    if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handleErrors();
    plaintext_len = len;

    /*
     * Finalise the decryption. Further plaintext bytes may be written at
     * this stage.
     */
    if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
        handleErrors();
    plaintext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}
