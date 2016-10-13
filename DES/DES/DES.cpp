/*----------------------------------
// 
// Data Encryption and Deciphering
// DES algorithm
//
// Programmed by: Kunlin Y
// Tested on Visual Studio 2015
//
-----------------------------------*/

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <conio.h>
#include "table.h"

/* Constant */
#define ENCRYPT_LENGTH	8			//length of each unit in encryption
#define DECIPHER_LENGTH 4			//length of each unit in decipher
#define MAX32			0xFFFFFFFF	//mask of 32 bits

/* Declaration */
typedef unsigned long long	bit64;
typedef unsigned long long	bit56;
typedef unsigned long long	bit48;
typedef unsigned int		bit32;
typedef unsigned int		bit28;D

/* File stream */
FILE *fin, *fout;

/* For debug */
inline void printBite(bit64 num)
{
	while (num)
	{
		printf("%d", num % 2);
		num >>= 1;
	}
	printf("\n");
}

/* Transfer from char to bit in Encrtption */
inline bit64 ToBit(
	char *in	// source string
	);

/* Transfer from char to bit in Deciphtering */
inline bit64 DeToBit(
	char *in	// source string
	);

/* Transfer from bit to char */
inline void ToBite(
	char *out,	// out string
	bit64 num	// source bits
	);

/* Permutation */
inline bit64 substitute(
	bit64 num,	// source bits
	const int *table,	// Permutation table
	size_t len	// bits length
	);

/* Bit recycle loop to left */
inline bit28 MoveLeft(
	bit28 key,	// source bits
	int len		// bits length
	);

/* Bit recycle loop to right */
inline bit28 MoveRight(
	bit28 key, 	// source bits
	int len		// bits length
	);

/* Divide bits into two parts */
inline void divide(
	bit64 num,	// source bits
	int len,	// length of each bits
	bit32 *L,	// left out bits
	bit32 *R	// right out bits
	);

/* S box */
inline bit32 SChange(
	bit48 num	// source bits
	);

/* F box */
inline bit32 FChange(
	bit32 num,	// source bits
	bit48 key	// secret key
	);

/* Key initialization */
inline void SetKey(
	char *in	// string of key
	);

/* Enryption */
inline void DES(
	char *message	// messages to be encrypted
	);

/* Deciphering */
inline void Decipher(
	char *message	// messages to be deciphered
	);

/* Initialization */
inline void init();

int main()
{
	init();
	system("pause");
	return 0;
}

/* Initialization */
inline void init()
{
	/* Set secret key */
	printf("Please input your secret key (8 digits):\n");
	char key[10000];
	scanf("%s", key);
	if (strlen(key) != 8)
	{
		printf("ERROR Key\n");
		return;
	}
	SetKey(key);

	/* Set mode Encryption or Deciphering */
	printf("Please input the mode (\"E\" for Encrypt, \"D\" for Decipher):\n");
	void(*p)(char*);
	int delta = 8;
	switch (getch())
	{
	case 'E': p = DES; delta = 8; break;
	case 'D': p = Decipher; delta = 16; break;
	default: printf("ERROR!\n"); return;
	}

	/* Load file */
	printf("Please input the path of the in file:\n");
	char message[10000], in[100], out[100];
	scanf("%s", in);
	printf("Please input the path of the out file:\n");
	scanf("%s", out);
	fin = freopen(in, "r", stdin);
	fout = freopen(out, "w", stdout);

	/* If success */
	if (!fin || !fout)
	{
		printf("Error open file!\n");
		return;
	}

	/* Read file */
	while (gets_s(message))
	{
		for (int i = 0; i < strlen(message); i += delta)
		{
			p(message + i);
		}
		printf("\n");
	}

	/* Close stream */
	fclose(stdin);
	fclose(stdout);
	fclose(fin);
	fclose(fout);
}

/* Transfer from char to bit in Encrtption */
inline bit64 ToBit(char *in)
{
	/* If valid */
	if (!in)
	{
		return 0;
	}

	/* Copy char* */
	char temp[8];
	memset(temp, ' ', 8 * sizeof(char));
	for (int i = 0; i < strlen(in) && i < ENCRYPT_LENGTH; i++)
	{
		temp[i] = in[i];
	}

	/* Transfer to bit */
	bit64 key = 0x0;
	for (int i = 0; i < ENCRYPT_LENGTH; i++)
	{
		key |= ((bit64)temp[i] << (ENCRYPT_LENGTH * i));
	}

	return key;
}

/* Transfer from char to bit in Deciphtering */
inline bit64 DeToBit(char *in)
{
	/* If valid */
	if (!in)
	{
		return 0;
	}

	/* Copy char* */
	char temp[64 / DECIPHER_LENGTH];
	memset(temp, ' ', 8 * sizeof(char));
	for (int i = 0; i < 64 / DECIPHER_LENGTH; i++)
	{
		if (in[i] >= 'A')
		{
			temp[i] = in[i] - '7';
		}
		else
		{
			if (in[i] >= '0')
			{
				temp[i] = in[i] - '0';
			}
		}
	}

	/* Transfer to bit */
	bit64 key = 0x0;
	for (int i = 0; i < 64 / DECIPHER_LENGTH; i++)
	{
		key |= ((bit64)temp[i] << (DECIPHER_LENGTH * i));
	}

	return key;
}

/* Transfer from bit to char */
inline void ToBite(char *out, bit64 num)
{
	if (strlen(out) <= ENCRYPT_LENGTH)
	{
		out = (char*)malloc(sizeof(char) * (ENCRYPT_LENGTH + 1));
	}
	memset(out, 0, sizeof(char) * (ENCRYPT_LENGTH + 1));

	for (int i = 0; i < ENCRYPT_LENGTH; i++)
	{
		out[i] = num & 0xFF;
		num >>= 8;
	}
}

/* Permutation */
inline bit64 substitute(bit64 num, const int *table, size_t len)
{
	bit64 out = 0;

	/* Calculation */
	for (int i = 0; i < len; i++)
	{
		out |= ((bit64)((num >> (table[i] - 1)) & 1) << i);
	}

	return out;
}

/* Bit recycle loop to left */
inline bit28 MoveLeft(bit28 key, int len)
{
	bit28 temp = 0;
	temp = key << (28 - len);	// right bits
	key = key >> len;			// left bits
	key |= temp;				// compare
	key &= 0x0FFFFFFF;			// delete highest four bits
	return key;
}

/* Bit recycle loop to right */
inline bit28 MoveRight(bit28 key, int len)
{
	bit28 temp = 0;
	temp = key >> (28 - len);	// right bits
	key = key << len;			// left bits
	key |= temp;				// compare
	return key;
}

/* Divide bits into two parts */
inline void divide(bit64 num, int len, bit32 *L, bit32 *R)
{
	*L = *R = 0;
	*L = num & MAX32;
	num >>= len;
	*R = num & MAX32;
}

/* S box */
inline bit32 SChange(bit48 num)
{
	bit32 key = 0;
	for (int i = 0; i < 8; i++)
	{
		bit32 x, y;
		x = (num >> 1) & 0x0F;						// the middle four bits
		y = (((num >> 5) & 1) << 1) | (num & 1);	// the first and the last bits
		key |= (S[i][y][x] << (i * 4));				// permutate
		num >>= 6;									// change to next
	}

	return key;
}

/* F box */
inline bit32 FChange(bit32 num, bit48 key)
{
	bit48 temp = substitute(num, E, sizeof(E) / sizeof(E[0]));
	temp ^= key;
	num = SChange(temp);

	return substitute(num, P, sizeof(P) / sizeof(P[0]));
}

/* Key initialization */
inline void SetKey(char *in)
{
	bit64 key = ToBit(in);
	bit28 C, D;

	key = substitute(key, PC1, sizeof(PC1) / sizeof(PC1[0]));
	divide(key, 28, &C, &D);
	for (int i = 0; i < 16; i++)
	{
		C = MoveLeft(C, Move[i]);
		D = MoveLeft(D, Move[i]);
		key = (bit64)C | ((bit64)D << 28);
		SubKey[i] = substitute(key, PC2, 48);
	}
}

/* Enryption */
inline void DES(char *message)
{
	bit64 BitMes = substitute(ToBit(message), IP, sizeof(IP) / sizeof(IP[0]));
	bit32 L, R, temp;
	divide(BitMes, 32, &L, &R);

	/* 16 rounds */
	for (int i = 0; i < 16; i++)
	{
		temp = R;
		R = FChange(R, SubKey[i]);
		R ^= L;
		L = temp;
	}

	BitMes = (bit64)L | ((bit64)R << 32);
	BitMes = substitute(BitMes, IPR, sizeof(IPR) / sizeof(IPR[0]));

	/* print encrypted message */
	for (int i = 0; i < 16; i++)
	{
		char temp = (0xF & (BitMes >> (i * 4)));
		temp += (temp > 9 ? '7' : '0');
		printf("%c", temp);
	}
}

/* Deciphering */
inline void Decipher(char *message)
{
	bit64 BitMes = substitute(DeToBit(message), IP, sizeof(IP) / sizeof(IP[0]));
	bit32 L, R, temp;
	divide(BitMes, 32, &L, &R);

	/* 16 rounds */
	for (int i = 15; i >= 0; i--)
	{
		temp = L;
		L = FChange(L, SubKey[i]);
		L ^= R;
		R = temp;
	}

	BitMes = (bit64)L | ((bit64)R << 32);
	BitMes = substitute(BitMes, IPR, sizeof(IPR) / sizeof(IPR[0]));

	/* print deciphered messages */
	for (int i = 0; i < 8; i++)
	{
		printf("%c", (0xFF & (BitMes >> (i * 8))));
	}
}