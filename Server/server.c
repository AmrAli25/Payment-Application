#include "server.h"
#include "time.h"

ST_accountsDB_t accountsDB[255];
ST_transaction_t transactionsDB[255] = { 0 };  // Initially with Zeros
int index;

void readingAccountDBfile(void)
{
	FILE* file;
	char ch = { 0 };
	file = fopen("AccountsDB.txt", "r");
	if (!file)
	{
		printf("File not Found");
	}
	char state[10] = { 0 };
	for (int i = 0; i < 255; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			ch = fgetc(file);
			if (ch == ',')
			{
				ch = fgetc(file);
				accountsDB[i].state =ch-'0';
				ch = fgetc(file);
				if (ch == ',')
				{
					for (int k = 0; k < 20; k++)
					{
						ch = fgetc(file);

						if (ch == '\n')
						{
							break;
						}
						accountsDB[i].primaryAccountNumber[k] = ch;
					}
				}

				break;
			}
			state[j] = ch;
		}
		float amount = atoi(state);
		accountsDB[i].balance = amount;
		
	}
	printf("########## Done Reading the Accounts Data Base ###########\n");
	fclose(file);
}

void updateAccountsDB(void)
{
	FILE* file = fopen("AccountsDB.txt", "w");
	if (file==NULL)
	{
		printf("Can't Upadate the Account Data base \n");
		return;
	}
	for (int i = 0; i < 255; i++)
	{
		if (accountsDB[i].primaryAccountNumber[0] == '\0')
		{
			break;
		}
		fprintf(file, "%0.1f,%d,%s\n", accountsDB[i].balance, accountsDB[i].state, accountsDB[i].primaryAccountNumber);
	}
	fclose(file);
}


/*This function will take all transaction data and validate its data, it contains all server logic.
It checks the account details and amount availability.
If the account does not exist return FRAUD_CARD, 
if the amount is not available will return DECLINED_INSUFFECIENT_FUND, 
if the account is blocked will return DECLINED_STOLEN_CARD, 
if a transaction can't be saved will return INTERNAL_SERVER_ERROR , else returns APPROVED.
It will update the database with the new balance.*/

EN_transState_t recieveTransactionData(ST_transaction_t* transData)
{
	if (isValidAccount(&transData->cardHolderData,&accountsDB[index])== ACCOUNT_NOT_FOUND)
	{
		transData->transState = FRAUD_CARD;
		return transData->transState;
	}
	else if (isAmountAvailable(&transData->terminalData.transAmount,&accountsDB[index])== LOW_BALANCE)
	{
		transData->transState = DECLINED_INSUFFECIENT_FUND;
		return transData->transState;

	}
	else if (isBlockedAccount(&accountsDB[index])== BLOCKED_ACCOUNT)
	{
		transData->transState = DECLINED_STOLEN_CARD;
		return transData->transState;
	}
	else 
	{
		transData->transState = APPROVED;
		accountsDB[index].balance = accountsDB[index].balance - transData->terminalData.transAmount;
	}
	if (saveTransaction(transData) == SAVING_FAILED)
	{
		transData->transState = INTERNAL_SERVER_ERROR;
		return transData->transState;
	}
	return transData->transState;

}


/*This function will take card data and validate if the account related to this card exists or not.
It checks if the PAN exists or not in the server's database (searches for the card PAN in the DB).
If the PAN doesn't exist will return ACCOUNT_NOT_FOUND and the account reference will be NULL, 
else will return SERVER_OK and return a reference to this account in the DB.*/

EN_serverError_t isValidAccount(ST_cardData_t* cardData, ST_accountsDB_t* accountRefrence)
{
	bool match = false;
	for (int i = 0; i < 255; i++)
	{
		// if the data base is not empty and has data
		if (cardData->primaryAccountNumber[0] != '\0')
		{
			bool flag = true;   // initialize the flag

			for (int j = 0; j < 20; j++)
			{
				if (cardData->primaryAccountNumber[j] != accountsDB[i].primaryAccountNumber[j])
				{
					flag = false;
					break;
				}
			}
			if (flag == true)
			{
				match = true;
				index = i;
				break;
			}
		}
		else  // there is no data to search in it 
		{
			break;
		}
	}
	if (match == false)
	{
		accountRefrence = NULL;
		return ACCOUNT_NOT_FOUND;
	}
	else
	{
		accountRefrence = &accountsDB[index];
		return SERVER_OK;
	}
}


/*This function takes a reference to the account into the databaseand verifies if it is blocked or not.
If the account is running it will return SERVER_OK, else if the account is blocked it will return BLOCKED_ACCOUNT.*/

EN_serverError_t isBlockedAccount(ST_accountsDB_t* accountRefrence)
{
	if (accountRefrence->state == BLOCKED)
	{
		return BLOCKED_ACCOUNT;
	}
	else
	{
		return SERVER_OK;
	}
}


/*This function will take terminal data and a reference to the account in 
the database and check if the account has a sufficient amount to withdraw or not.
It checks if the transaction's amount is available or not.
If the transaction amount is greater than the balance in the database will return LOW_BALANCE, else will return SERVER_OK. */

EN_serverError_t isAmountAvailable(ST_terminalData_t* termData, ST_accountsDB_t* accountRefrence)
{
	if (termData->transAmount > accountRefrence->balance)
	{
		return LOW_BALANCE;
	}
	else
	{
		return SERVER_OK;
	}
}

/*This function will store all transaction data in the transactions database.
It gives a sequence number to a transaction, this number is incremented once a transaction is processed into the server,
you must check the last sequence number in the server to give the new transaction a new sequence number.
It saves any type of transaction, APPROVED, DECLINED_INSUFFECIENT_FUND, DECLINED_STOLEN_CARD, FRAUD_CARD, INTERNAL_SERVER_ERROR.
It will list all saved transactions using the listSavedTransactions function.
Assuming that the connection between the terminal and server is always connected, then it will return SERVER_OK.*/

EN_serverError_t saveTransaction(ST_transaction_t* transData)
{
	FILE* seq = fopen("SEQ.txt", "r");
	if (seq == NULL)
	{
		return SAVING_FAILED;
	}
	int seqnum=0;
	fscanf(seq, "%d", &seqnum);
	fclose(seq);
	seq = fopen("SEQ.txt", "w");
	fprintf(seq, "%d", seqnum + 1);
	fclose(seq);
	transData->transactionSequenceNumber = seqnum + 1;
	FILE* file = fopen("TransactionsDB.txt", "a");
	if (file == NULL)
	{
		return SAVING_FAILED;
	}
	fprintf(file, "\n##########################################################\n");
	fprintf(file, "Transaction Sequence Number: %d\n",transData->transactionSequenceNumber);
	fprintf(file, "Transaction Date: %s\n", transData->terminalData.transactionDate);
	fprintf(file, "Transaction Amount: %0.1f\n", transData->terminalData.transAmount);
	fprintf(file, "Transaction State: ");
	if (transData->transState == APPROVED)
	{
		fprintf(file, "APPROVED\n");
	}
	else if (transData->transState == DECLINED_INSUFFECIENT_FUND)
	{
		fprintf(file, "DECLINED_INSUFFECIENT_FUND\n");
	}
	else if (transData->transState == DECLINED_STOLEN_CARD)
	{
		fprintf(file, "DECLINED_STOLEN_CARD\n");
	}
	else if (transData->transState == INTERNAL_SERVER_ERROR)
	{
		fprintf(file, "INTERNAL_SERVER_ERROR\n");
	}
	else if (transData->transState == FRAUD_CARD)
	{
		fprintf(file, "FRAUD_CARD\n");
	}
	fprintf(file, "Terminal Max Amount: %0.1f\n", transData->terminalData.maxTransAmount);
	fprintf(file, "Cardholder Name: %s", transData->cardHolderData.cardHolderName);
	fprintf(file, "PAN: %s\n", transData->cardHolderData.primaryAccountNumber);
	fprintf(file, "Card Expiration Date: %s", transData->cardHolderData.cardExpirationDate);	
	fprintf(file, "\n##########################################################\n");
	fclose(file);
	return SERVER_OK;
}

void listSavedTransactions(void)
{
	FILE* file = fopen("TransactionsDB.txt", "r");
	char ch;
	if (file == NULL)
	{
		printf("File Not Found");
	}
	do {
		ch = fgetc(file);
		printf("%c", ch);

	} while (ch != EOF);
	fclose(file);
}
