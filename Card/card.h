#ifndef _CARD_H_
#define _CARD_H_


/************************ Includes ****************************/

#include "stdint.h"
#include "stdio.h"
#include "string.h"

/************************** Variabls *************************/

typedef struct ST_cardData_t
{
	uint8_t cardHolderName[25];
	uint8_t primaryAccountNumber[20];
	uint8_t cardExpirationDate[6];
}ST_cardData_t;

typedef enum EN_cardError_t
{
	CARD_OK, WRONG_NAME, WRONG_EXP_DATE, WRONG_PAN
}EN_cardError_t;

//extern int pan;

/********************** Functions ***************/

EN_cardError_t getCardHolderName(ST_cardData_t* cardData);
EN_cardError_t getCardExpiryDate(ST_cardData_t* cardData);
EN_cardError_t getCardPAN(ST_cardData_t* cardData);
long long generate_luhncard_pan();






#endif // !_CARD_H_

