#include "app.h"

void appStart(void) 
{
	float max;

	printf("\n Reading AccountsDB.....\n");
	readingAccountDBfile();
	printf("\n Succesfully loaded AccountsDB........\n");
	ST_cardData_t cardData;
	ST_terminalData_t termData;
	ST_transaction_t transData;
	EN_terminalError_t termReturn;
	EN_cardError_t cardReturn = getCardHolderName(&cardData);
	while (cardReturn == WRONG_NAME) 
	{
		printf("\n ******* WRONG_NAME *******\n");
		printf("Re-enter:\n");
		cardReturn = getCardHolderName(&cardData);
	}
	cardReturn = getCardExpiryDate(&cardData);
	while (cardReturn == WRONG_EXP_DATE)
	{
		printf("\n******* WRONG_EXP_DATE *******\n");
		printf("Re-enter:\n");
		cardReturn = getCardExpiryDate(&cardData);
	}
	cardReturn = getCardPAN(&cardData);
	while (cardReturn == WRONG_PAN)
	{
		printf("\n******* WRONG_PAN *******\n");
		printf("Re-enter:\n");
		cardReturn = getCardPAN(&cardData);
	}
	// if i want the user to enter the max amount
	//printf("%f", max);
	//scanf("%f", &max);
	termReturn = setMaxAmount(&termData,50000);
	while (termReturn == INVALID_AMOUNT) 
	{
		printf("\n******* INVALID AMOUNT *******\n");
		printf("Re-enter:\n");
		termReturn = setMaxAmount(&termData,50000);
	}
	termReturn = getTransactionDate(&termData);
	while (termReturn == WRONG_DATE)
	{
		printf("\n******* WRONG_DATE *******\n");
		printf("Re-enter:\n");
		termReturn = getTransactionDate(&termData);
	}
	termReturn = isCardExpired(cardData, termData);
	if (termReturn == EXPIRED_CARD)
	{
		printf("\n******* EXPIRED_CARD *******\n");
		printf("\n******* INTERNAL_SERVER_ERROR *******\n");

		return;
	}
	termReturn = getTransactionAmount(&termData);
	while (termReturn == INVALID_AMOUNT) 
	{
		printf("\n******* INVALID_AMOUNT *******\n");
		printf("Re-enter:\n");
		termReturn = getTransactionAmount(&termData);
	}
	termReturn = isBelowMaxAmount(&termData);
	if (termReturn == EXCEED_MAX_AMOUNT)
	{
		printf("\n******* DECLINED EXCEED_MAX_AMOUNT *******\n");
		printf("\n******* INTERNAL_SERVER_ERROR *******\n");
		return;
	}
	transData.cardHolderData = cardData;
	transData.terminalData = termData;
	EN_transState_t transSt = recieveTransactionData(&transData);
	if (transSt == DECLINED_STOLEN_CARD) 
	{
		printf("\n******* DECLINED STOLEN CARD/INVALID ACCOUNT *******\n");
		saveTransaction(&transData);

	}
	else if (transSt == DECLINED_INSUFFECIENT_FUND) 
	{
		printf("\n******* DECLINED_INSUFFECIENT_FUND *******\n");
		saveTransaction(&transData);
	}
	else if (transSt == FRAUD_CARD)
	{
		printf("\n******* FRAUD_CARD *******\n");
		saveTransaction(&transData);
	}
	else if (transSt == APPROVED)
	{
		printf("\n******* APPROVED *******\n");
	}
	printf("\nUPDATING ACCOUNTS DB.....\n");
	updateAccountsDB();
	printf("\nSAVING TRANSACTION.......\n");
	listSavedTransactions();
	printf("\n TRANSACTION SAVED.......\n");
}
