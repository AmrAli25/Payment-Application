#include "card.h"




//	This function will ask for the cardholder's name and store it into card data.
//	Card holder name is 24 characters string max and 20 min.
//	If the cardholder name is NULL, less than 20 characters or more than 24 will return WRONG_NAME error, 
//	else return CARD_OK.

EN_cardError_t getCardHolderName(ST_cardData_t* cardData)
{
	char name[50] = { 0 };
	printf("What is the Card Holder Name [20 or 24 Character] : ");
	fgets(name, sizeof(name), stdin);
	if ((strlen(name) < 20) || (strlen(name) > 25))
	{
		return WRONG_NAME;
	}
	else
	{
		for (int i = 0; i < 25; i++)
		{
			cardData->cardHolderName[i] = name[i];
		}
		return CARD_OK;
	}
};

//	This function will ask for the card expiry date and store it in card data.
//	Card expiry date is 5 characters string in the format "MM/YY", e.g "05/25".
//	If the card expiry date is NULL, less or more than 5 characters, 
//	or has the wrong format will return WRONG_EXP_DATE error, else return CARD_OK.

EN_cardError_t getCardExpiryDate(ST_cardData_t* cardData)
{
	char num[50];
	printf("What is the Card Expiry Date [MM/YY] : ");
	fgets(num, sizeof(num), stdin);
	if ((strlen(num) < 6) || (strlen(num) > 6) || num[2] != '/')
	{
		return WRONG_EXP_DATE;
	}
	else
	{
		for (int i = 0; i < 6; i++)
		{
			cardData->cardExpirationDate[i] = num[i];
		}
		return CARD_OK;
	}

};

//	This function will ask for the card's Primary Account Number and store it in card data.
//	PAN is 20 characters alphanumeric only string 19 character max, and 16 character min.
//	If the PAN is NULL, less than 16 or more than 19 characters, will return WRONG_PAN error, else return CARD_OK.

EN_cardError_t getCardPAN(ST_cardData_t* cardData)
{
	int pan = 16;
	int answer = 0;
	printf("Do you want to enter the card PAN or generate one [1(enter user)/2(generate)]");
	scanf("%d", &answer);
	if (answer == 1)
	{
		char PAN[20] = { 0 };
		printf("What is the Card PAN  [16 or 19 Characters] : ");
		scanf("%s",PAN);
		if ((strlen(PAN) < 16) || (strlen(PAN) > 20))
		{
			return WRONG_PAN;
		}
		else
		{
			for (int i = 0; i < 20; i++)
			{
				cardData->primaryAccountNumber[i] = PAN[i];
			}
			return CARD_OK;
		}

	}
	else
	{
		long long PAN_G = generate_luhncard_pan();
		char number[20] = { 0 };

		for (int i = 0; i < pan; i++)
		{

			int num = (PAN_G % 10);
			number[pan - 1 - i] = num + '0';
			PAN_G = PAN_G / 10;
		}
		for (int i = 0; i < 20; i++)
		{
			cardData->primaryAccountNumber[i] = number[i];
		}
		return CARD_OK;
	}
	
};


// this function to generate the Card number
long long generate_luhncard_pan()
{
	int pan = 16;
	int arr[20] = { 0 };            // An array to save the LUHN pan
	int a, sum;
	srand(time(NULL));
	//printf("How long is the PAN [16 to 19 digit] :");
	//scanf("%d", &pan);

	if ((pan > 15) && (pan < 20))
	{

		// start with a digit between 1 and 9
		long long cardnumber;
//	X:
		sum = 0;
		cardnumber = 1 + rand() % 9;
		arr[0] = cardnumber;
		// add the remaining  digits  and use luhn number algorithm
		for (int i = 1; i < pan; i++)          
		{
			cardnumber = cardnumber * 10 + rand() % 10;
			arr[i] = cardnumber % 10;

		}
		return cardnumber;
		/*
		for (int i = pan - 2; i >= 0; i = i - 2)
		{
			a = arr[i] * 2;
			if (a > 9)
			{
				arr[i] = a - 9;
			}
			else
			{
				arr[i] = a;
			}

		}
		for (int i = 0; i < pan; i++)
		{
			a = arr[i];
			sum = sum + a;
		}
		if (sum % 10 == 0)   // check for the sum of the PAN number if 0 then it's correct else begin from X
		{
			return cardnumber;
		}
		else
		{
		//	goto X;
		}*/
	}
	else
	{
		printf("Wrong number of digit entered\n");
		return WRONG_PAN;
	}
}




