#include "StringHelper.h"
#include <string.h>
#include "ctype.h"
#include "cstdlib"

int StringHelper::indexOf(char *str, char key)
{
	int str_size = strlen(str);

	for (int i = 0; i < str_size; i++)
	{
		if (str[i] == key)
		{
			return i;
		}
	}

	return -1;
}

void StringHelper::empty(char *str, int str_sz)
{
	for (int i = 0; i < str_sz; i++)
	{
		str[i] = 0;
	}
}

char *StringHelper::substring(char *str, int start_idx, int end_idx, char* oResult, int result_sz)
{
	// Check if size is 0 or negative
	if (end_idx <= start_idx)
	{
		return oResult;
	}

	//char t_str[32];
	StringHelper::empty(oResult, result_sz);
	for (int i = start_idx, j = 0; i < end_idx; i++, j++)
	{
		oResult[j] = str[i];
	}
	//strncat(oResult, t_str, result_sz);
}

int StringHelper::parseToInt(char *str)
{
	// Make sure val is parseable into an int
	char valCmp[6];
	StringHelper::empty(valCmp, 6);
	for (int i = 0; i < strlen(str); i++)
	{
		if (isdigit(str[i])) // Test if digit
		{
			valCmp[i] = str[i];
		}
		else if (i == 0 && str[i] == '-') // Test if negative sign
		{
			valCmp[i] = str[i];
		}
		else // Fail
		{
			return 0;
		}
	}

	// Check and compare the results to determine if val is an int
	if (strlen(valCmp) == strlen(str))
	{
		return atoi(str);
	}

	return 0;
}

long StringHelper::parseToLong(char *str)
{
	// Make sure val is parseable into an int
	char valCmp[13];
	StringHelper::empty(valCmp, 13);
	for (int i = 0; i < strlen(str); i++)
	{
		if (isdigit(str[i])) // Test if digit
		{
			valCmp[i] = str[i];
		}
		else if (i == 0 && str[i] == '-') // Test if negative sign
		{
			valCmp[i] = str[i];
		}
		else // Fail
		{
			return 0;
		}
	}

	// Check and compare the results to determine if val is a long
	if (strlen(valCmp) == strlen(str))
	{
		return atol(str);
	}

	return 0;
}

//int StringHelper::parseToInt(char *str)
//{
//	// Make sure val is parseable into an int
//	char valCmp[10];
//	StringHelper::empty(valCmp, 10);
//	for(int i = 0; i < strlen(str); i++)
//	{
//		if(isDigit(str[i])) // Test if digit
//		{
//			valCmp[i] = str[i];
//		}
//		else if( i == 0 && str[i] == '-') // Test if negative sign
//		{
//			valCmp[i] = str[i];
//		}
//		else // Fail
//		{
//			return 0;
//		}
//	}
//
//	// Check and compare the results to determine if val is an int
//	if(strlen(valCmp) == strlen(str))
//	{
//		return atoi(str);
//	}
//
//	return 0;
//}

//bool StringHelper::equals(char *str1, char *str2)
//{
//	int size1 = strlen(str1);
//	//Serial.print("str1 length: ");
//	//Serial.println(size1);
//	int size2 = strlen(str2);
//	//Serial.print("str2 length: ");
//	//Serial.println(size2);
//
//	if(size1 == 0 && size2 == 0)
//	{
//		/*Serial.println("Size is 0");
//		Serial.println("END equals()");*/
//		return true;
//	}
//
//	if(size1 != size2)
//	{
//		/*Serial.println("Not Same Size");
//		Serial.println("END equals()");*/
//
//		//while(1){}
//		return false;
//	}
//
//	for(int i = 0; i < size1; i++)
//	{
//		if(str1[i] != str2[i])
//		{
//			/*Serial.println("Char not equal");
//			Serial.println("END equals()");*/
//			//while(1){}
//			return false;
//		}
//	}
//
//	/*Serial.println("Returned TRUE");
//	Serial.println("END equals()");*/
//	//while(1){}
//	return true;
//}
//
//bool StringHelper::startsWith(char *target_str, char *test_str)
//{
//	/*Serial.println("BEGIN startsWith()");
//	Serial.print("target_str: ");
//	Serial.println(target_str);
//	Serial.print("test_str: ");
//	Serial.println(test_str);*/
//
//	int target_sz = strlen(target_str);
//	int test_sz = strlen(test_str);
//	/*Serial.print("target_str length: ");
//	Serial.println(strlen(target_str));
//	Serial.print("test_str length: ");
//	Serial.println(strlen(test_str));*/
//
//	if(test_sz > target_sz)
//	{
//		/*Serial.println("Returned FALSE");
//		Serial.println("END startsWith()");*/
//		return false;
//	}
//
//	if(equals(target_str, test_str))
//	{
//		/*Serial.println("Returned TRUE");
//		Serial.println("END startsWith()");*/
//		return true;
//	}
//
//	for(int i = 0; i < test_sz; i++)
//	{
//		if((char)target_str[i] != (char)test_str[i])
//		{
//			/*Serial.println("Returned FALSE");
//			Serial.println("END startsWith()");*/
//			return false;
//		}
//	}
//
//	/*Serial.println("Returned TRUE");
//	Serial.println("END startsWith()");*/
//	return true;
//}