#ifndef _DATE_H_
#define _DATE_H_

#include "includes.h"

class Date 
{
private:
	int m_day, m_month, m_year; // day , month , year of the Date
	unsigned m_day_number; // number of days from 00/00/0001
public:
	Date(void);
	Date(const Date&);					// CC
	Date(char d, char m, int y);		// Constructor;
	Date(const string&);				// constructor string
	void set_day_number(char d, char m, int y);
	void set_day_month_year(unsigned n);
	void add(int n);					// add n days to the date
	const string GetDate(void) const;	// returns the date as a string
	friend ostream& operator<<(ostream&, const Date&);
	int operator-(const Date& other) const; // operator - for 2 dates
	bool operator<=(const Date& other) const;// operator <= for 2 dates
	bool operator>=(const Date& other) const; // operator >= for 2 dates
	bool operator<(const Date& other) const;// operator < for 2 dates
	bool operator==(const Date& other) const; // operator == for 2 dates
};
bool is_leap_year(int year);					// return true if year is leap year, false otherwise
int num_days_month(int month, int year);		// return number of days in month (year is provided to check leap year)
#endif // _DATE_H_