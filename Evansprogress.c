#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/err.h>
#include <string.h>
#include <stdbool.h>

char* concat(const char *s1, const char *s2);
void handleErrors(void);
int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext);


int main (void)
{
    unsigned char *correctCiphertext = (unsigned char *)"6b642b4d232d28fb9272d3aae053d6410ef9dfb267bbb9d9adcfee0f2d823f14";
    //char givenCipher[64]; 
    // strncpy(givenCipher, &correctCiphertext[81], 64);
    //givenCipher[] = 6b642b4d232d28fb9272d3aae053d6410ef9dfb267bbb9d9adcfee0f2d823f14;
    char givenCipher[] = "6b642b4d232d28fb9272d3aae053d6410ef9dfb267bbb9d9adcfee0f2d823f14";

    char rawCipher[256]; //ciphertexts from the list of words

    char rawHex1[48]; //first line of ciphertext
    char rawHex2[48]; //second line of ciphertext

    char str[16];
    int numChar;
    
    FILE *test;
    FILE *fp;
    
    char* wordsFile = "/home/seed/words.txt";
    char* testFile = "/home/seed/testOutput.txt";

    fp = fopen(wordsFile, "r");
    test = fopen(testFile, "w");
    if (fp == NULL)
    {
        printf("Could not open file %s", wordsFile);
        return 1;
    }
    

    /*
     * Set up the key and iv. Do I need to say to not hard code these in a
     * real application? :-)
     */
	
    bool matched = false;
    while (fgets(str, 16, fp) != NULL && matched == false) //Will eventually be while ciphers don't match
    {

        numChar = strlen(str) - 1;
	
        //adds # signs to the end of a word to make it a 128 bit key
        for (int i = numChar; i < 16; i++)
        {
            str[i] = '#';
        }

        
	    /* A 128 bit key */
	    unsigned char *key = (unsigned char *) str;
            
	    /* A 128 bit IV */
	    unsigned char *iv = (unsigned char *)"aabbccddeeff00998877665544332211";

	    /* Message to be encrypted */
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

	    

            freopen("testOutput.txt", "w+", test); //writing access to testOutput.txt
            BIO_dump_fp (test, (const char *)ciphertext, ciphertext_len); //write output to file

            fclose(test);

            fopen(testFile, "r");
            fread(&rawCipher, sizeof(char), 256, test);

	    
           
            strncpy(rawHex1, &rawCipher[7], 47); //7 is the start of cipher text in hex and 81 is the end of it. These indices are used to eliminate extra garbage
            strncpy(rawHex2, &rawCipher[81], 47);

            int len;
            len = strlen(rawHex1);

            //remove spaces from first line of ciphertext
            for(int i=0; i<len; i++)
	    {
		if(rawHex1[i]==' ')
		{
			for(int j=i; j<len; j++)
			{
				rawHex1[j]=rawHex1[j+1];
			}
		len--;
		}
	    }

            //remove dashes from first line of ciphertext
            len = strlen(rawHex1);  
            for(int i=0; i<len; i++)    
	    {
		if(rawHex1[i]=='-')
		{
			for(int j=i; j<len; j++)
			{
				rawHex1[j]=rawHex1[j+1];
			}
		len--;
		}
	    }

            //remove spaces from second line of ciphertext
            len = strlen(rawHex2);
            for(int i=0; i<len; i++)
	    {
		if(rawHex2[i]==' ')
		{
			for(int j=i; j<len; j++)
			{
				rawHex2[j]=rawHex2[j+1];
			}
		len--;
		}
	    }

            //remove dashes from second line of ciphertext
            len = strlen(rawHex2);
            for(int i=0; i<len; i++)
	    {
		if(rawHex2[i]=='-')
		{
			for(int j=i; j<len; j++)
			{
				rawHex2[j]=rawHex2[j+1];
			}
		len--;
		}
	    }

           // printf("%s", "Printing cipher: \n");
            printf("%s \n", rawHex1);  //print first line of current ciphertext
            printf("%s \n", rawHex2);  //print second line of current ciphertext

            char* currentCipherText = concat(rawHex1, rawHex2); //concatenate the two parts of the hex ciphertext
            printf("\n");
            printf("%s", "current Ciphertext:");
            printf("%s \n", currentCipherText);
            printf("\n");
            printf("%s", "Correct Ciphertext:");
            printf("%s \n", correctCiphertext);
            printf("\n");
            printf("%s \n", str); //print the current word
            printf("\n");

            // compare the correct ciphertext to the ciphertext produced by the current word being tested
            if (strcmp(correctCiphertext, currentCipherText) == 0)
            {
            	printf("Hey they matched! \n");
                printf("The key is: ");
                printf("%s \n", str); //print the key
                matched = true;
 
            }
/*
            if (strncmp(correctCiphertext, currentCipherText, 16) == 0)
            {
            	printf("Hey they matched!");
                matched = true;
 
            }
*/
            printf("\n");
    }

    fclose(fp);
    fclose(test); 
    return 0;
}

//combines two strings
char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

void handleErrors(void)
{
    ERR_print_errors_fp(stderr);
    abort();
}

/*
 * Handles the encryption of the words
 */
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
