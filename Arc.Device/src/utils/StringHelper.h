#pragma once

class StringHelper
{
private:


public:
	static int indexOf(char *str, char key);
	static char *substring(char *str, int start_idx, int end_idx, char* oResult, int result_sz);
	static void empty(char *str, int str_sz);
	static int parseToInt(char *str);
	static long parseToLong(char *str);
	//static bool startsWith(char *target_str, char *test_str);
	//static bool equals(char *str1, char *str2);
};

