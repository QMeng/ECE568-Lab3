#include <stdio.h>


#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "lib/encoding.h"

void addPadding(char * secret, int length) {
	int i;
	char padding[20] = "";
	for (i = 0; i < length; i++) {
		padding[i] = secret[i];
	}
	for (i = length; i < 20; i++) {
		padding[i] = '0';
	}
	strcpy(secret, padding);
}

uint8_t charValue(char c) {
	if(c >= '0' && c <= '9') {
		return c - '0';
	} else if(c >= 'a' && c <= 'f') {
		return c - 'a' + 10;
	} else if(c >= 'A' && c <= 'F') {
		return c - 'A' + 10;
	}
	return 255;
}

void generateHex(char *secret, uint8_t * solution) {
    uint8_t * c;
    int i;
    for(i=0, c = (uint8_t *) secret; i < 10; i++, c += 2) {
        solution[i] = (charValue(*c) << 4) | charValue(*(c + 1));
    }
    solution[10] = 0;
}

int
main(int argc, char * argv[])
{
	if ( argc != 4 ) {
		printf("Usage: %s [issuer] [accountName] [secretHex]\n", argv[0]);
		return(-1);
	}

	char *	issuer = argv[1];
	char *	accountName = argv[2];
	char *	secret_hex = argv[3];

	assert (strlen(secret_hex) <= 20);

	char * encodedName = urlEncode(accountName);
	char * encodedIssuer = urlEncode(issuer);

	printf("\nIssuer: %s\nAccount Name: %s\nSecret (Hex): %s\n\n", 
		issuer, accountName, secret_hex);

	int secretLength = strlen(secret_hex);
	if (secretLength < 20) {
		addPadding(secret_hex, secretLength);
	}

	// Create an otpauth:// URI and display a QR code that's compatible
	// with Google Authenticator
	
	generateHex(secret_hex, secret_hex);
	char encodedSecret[20];
	base32_encode(secret_hex, 10, encodedSecret, 20);

	printf(encodedSecret);

	char URI1[200];
	sprintf(URI1, "otpauth://hotp/%s?issuer=%s&secret=%s&counter=1", encodedName, encodedIssuer, encodedSecret);
	displayQRcode(URI1);

	char URI2[200];
	sprintf(URI2, "otpauth://totp/%s?issuer=%s&secret=%s&period=30", encodedName, encodedIssuer, encodedSecret);
	displayQRcode(URI2);

	return (0);
}
