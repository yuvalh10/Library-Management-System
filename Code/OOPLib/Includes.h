#ifndef _INCLUDES_H_
#define _INCLUDES_H_

#define   _CRTDBG_MAP_ALLOC
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <ctime>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <sstream>
#include <fstream>
#include <crtdbg.h>
#include <stdbool.h>
#include <string>
#include <cstdlib>
#include <exception>
#include <array>
#include <stack>

#define MAX_ATTEMPTS 5
#define MIN_ID_LENGTH 4
#define MAX_ID_LENGTH 20
#define PASSWORD_LENGTH 1
#define NAME_LENGTH 1
#define ISBN_SIZE 13
#define NUM_DAYS_YEAR 365
#define DATE_LENGTH 10
#define DATE_FORMAT 23
#define MORE 10
#define FIELDS 6

// MACROS
//returns true if the ISBN is not exactly ISBN_SIZE characters long or contains any non-digit characters.
#define NOT_VALID_ISBN(ISBN) (ISBN.size() != ISBN_SIZE || ISBN.find_first_not_of("0123456789") != string::npos)
//returns true if category is empty or contains any numeric characters.
#define NOT_VALID_CATEGORY(category) (category.empty() || category.find_first_of("0123456789") != string::npos)
//returns true if copies is empty or contains any non - digit characters.
#define NOT_VALID_NUMBER(copies) (copies.empty() || copies.find_first_not_of("0123456789") != string::npos)
//returns true if substr is found anywhere within str.
#define MATCH(str, substr) ((str).find(substr) != string::npos)
//returns true if i is at least 4 and str contains any non - digit characters.
#define IS_NUMBER(i,str) (((i >= 4) && (str.find_first_not_of("0123456789") != string::npos)) || str.empty())
// determines if input is exactly ISBN_SIZE digits or a name (non-numeric or wrong size).
#define ISBN_OR_NAME(input) (input.size() == ISBN_SIZE || input.find_first_not_of("0123456789") != string::npos)
// check if the priority is valid
#define VALID_PRIOTITY(priority) (priority.empty() || priority.find_first_not_of("0123456789") != string::npos || stoi(priority) < 1 || stoi(priority) > 5)

using namespace std;

#define   _CRTDBG_MAP_ALLOC

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

#endif // _INCLUDES_H_