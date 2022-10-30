#include "terminal.h"
#include "time.h"

//	This function will ask for the transaction data and store it in terminal data.
//	Transaction date is 10 characters string in the format DD / MM / YYYY, e.g 25 / 06 / 2022.
//	If the transaction date is NULL, less than 10 characters or wrong format will return WRONG_DATE error,
//	 else return TERMINAL_OK.

EN_terminalError_t getTransactionDate(ST_terminalData_t* termData)
{
	time_t t;
	time(&t);
	char tm[11] = { 0 };
	char tmp1, tmp2, tmp3, tmp4;

	// Convert the system time to string  and Make it like the format DD/MM/YYYY.

	strftime(tm, sizeof(tm), "%x", localtime(&t));
	tmp1 = tm[0];
	tmp2 = tm[1];
	tmp3 = tm[3];
	tmp4 = tm[4];

	tm[0] = tmp3;
	tm[1] = tmp4;
	tm[3] = tmp1;
	tm[4] = tmp2;
	tm[8] = tm[6];
	tm[9] = tm[7];
	tm[6] = '2';
	tm[7] = '0';

	// Copy the the date in the transaction date array

	for (int i = 0; i < 11; i++)
	{
		termData->transactionDate[i] = tm[i];	
	}
	return TERMINAL_OK;

	// If you want the user to enter the date   

	/*char date[100] = {0};
	printf("What is the Transaction Date [DD/MM/YYYY] : ");
	fgets(date, sizeof(date), stdin);
	if ((strlen(date) < 11) || (strlen(date) > 11) || (date[2] != '/') || (date[5] != '/'))
	{
		return WRONG_DATE;
	}
	else
	{
		for (int i = 0; i < 11; i++)
		{
			termData->transactionDate[i] = date[i];
			return TERMINAL_OK;
		}
	}*/
}


//This function compares the card expiry date with the transaction date.
//If the card expiration date is before the transaction date will return EXPIRED_CARD, else return TERMINAL_OK.

EN_terminalError_t isCardExpired(ST_cardData_t cardData, ST_terminalData_t termData)
{
	int CardMonth = (cardData.cardExpirationDate[0] - '0') * 10 + (cardData.cardExpirationDate[1] - '0');
	int CardYear = (cardData.cardExpirationDate[3] - '0') * 10 + (cardData.cardExpirationDate[4] - '0');
	int termMonth = (termData.transactionDate[3] - '0') * 10 + (termData.transactionDate[4] - '0');
	int termYear = (termData.transactionDate[8] - '0') * 10 + (termData.transactionDate[9] - '0');
	if (CardYear > termYear)
	{
		return TERMINAL_OK;
	}
	else if ((CardYear == termYear) && (CardMonth >= termMonth))
	{
		return TERMINAL_OK;
	}
	else
	{
		return EXPIRED_CARD;
	}
}

//This function asks for the transaction amount and saves it into terminal data.
//If the transaction amount is less than or equal to 0 will return INVALID_AMOUNT, else return TERMINAL_OK.

EN_terminalError_t getTransactionAmount(ST_terminalData_t* termData)
{
	float amount = -1;
	printf("Enter the transaction amount : ");
	scanf("%f", &amount);
	if (amount <= 0)
	{
		return INVALID_AMOUNT;
	}
	else 
	{
		termData->transAmount = amount;
		return TERMINAL_OK;
	}
}


//This function sets the maximum allowed amount into terminal data.
//Transaction max amount is a float number.
//If transaction max amount less than or equal to 0 will return INVALID_MAX_AMOUNT error, else return TERMINAL_OK.

EN_terminalError_t setMaxAmount(ST_terminalData_t* termData, float maxAmount)
{
	
	if (maxAmount <= 0)
	{
		return INVALID_MAX_AMOUNT;
	}
	else
	{
		termData->maxTransAmount = maxAmount;
		return TERMINAL_OK;
	}
}

//This function compares the transaction amount with the terminal max amount.
//If the transaction amount is larger than the terminal max amount will return EXCEED_MAX_AMOUNT, else return TERMINAL_OK.

EN_terminalError_t isBelowMaxAmount(ST_terminalData_t* termData)
{
	if (termData->transAmount > termData->maxTransAmount)
	{
		return EXCEED_MAX_AMOUNT;
	}
	else
	{
		return TERMINAL_OK;
	}
}


// This function checks if the PAN is a Luhn number or not.
//If PAN is not a Luhn number will return INVALID_CARD, else return TERMINAL_OK.

EN_terminalError_t isValidCardPAN(ST_cardData_t* cardData)
{
	int pan = 16;
	char cardpan[20] = { 0 };
	int a, sum = 0;
	for (int i = 0; i < strlen(cardData->primaryAccountNumber); i++)
	{
		cardpan[i] = cardData->primaryAccountNumber[i];
	}
	for (int i = pan - 2; i >= 0; i = i - 2)
	{
		a = (cardpan[i] - '0') * 2;
		if (a > 9)
		{
			cardpan[i] = (a - 9) + '0';
		}
		else
		{
			cardpan[i] = a + '0';
		}

	}
	for (int i = 0; i < pan; i++)
	{
		a = cardpan[i] - '0';
		sum = sum + a;
	}
	printf("%d", sum);
	if (sum % 10 == 0)
	{
		printf("\nthe PAN is Lunh Number");
		return TERMINAL_OK;
	}
	else
	{
		return INVALID_CARD;
	}
}
