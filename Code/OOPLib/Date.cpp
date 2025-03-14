#include "includes.h"
#include "Date.h"

Date::Date(char day, char month, int year) // Ctor
{
	m_day = day; m_month = month; m_year = year;
	set_day_number(m_day, m_month, m_year);
}

Date::Date(const Date& d) // Copy Constructor
{
	m_day = d.m_day;
	m_month = d.m_month;
	m_year = d.m_year;
	m_day_number = d.m_day_number;
}

Date::Date(const string& dateStr) // creates an object of date from string (Ctor)
{
	stringstream ss(dateStr);
	string day, month, year;

	getline(ss, day, '/');
	getline(ss, month, '/');
	getline(ss, year);

	m_day = stoi(day);  // Convert day string to integer
	m_month = stoi(month); // Convert month string to integer
	m_year = stoi(year);
	set_day_number(m_day, m_month, m_year);
}

Date::Date() // Default constructor to set current date
{
	time_t now = time(0); // get the current time
	struct tm* local_time = localtime(&now);
	m_day = local_time->tm_mday;
	m_month = local_time->tm_mon + 1;  // tm_mon is 0-based
	m_year = local_time->tm_year + 1900;  // tm_year is years since 1900
	set_day_number(m_day, m_month, m_year);
}

 void Date::set_day_number(char d, char m, int y)
 { // a given function from lab 4, sets the days that been passed from 1/1/0000
	 m_day_number = 0;
	 for (int i = 0; i < y; i++)
		 m_day_number += NUM_DAYS_YEAR + is_leap_year(i);
	 for (int i = 1; i < m; i++)
		 m_day_number += num_days_month(i, y);
	 m_day_number += m_day - 1;
 }

 void Date::set_day_month_year(unsigned n)
 { // converts days count into year, month, and day.
	 m_day_number = n;
	 for (m_year = 0; n >= NUM_DAYS_YEAR; m_year++)
		 n -= NUM_DAYS_YEAR + is_leap_year(m_year); // how many years has passed
	 // which month is the given date
	 for (m_month = 1; n >= num_days_month(m_month, m_year); m_month++) 
		 n -= num_days_month(m_month, m_year);
	 m_day = n + 1;
 }

 bool is_leap_year(int year)
 { // check if the year is a leap year
	 if (year % 4 == 0)
	 {
		 if (year % 100 != 0)
			 return true;
		 else if (year % 400 == 0)
			 return true;
	 }			 
	return false;
 }

 int num_days_month(int month, int year)
 { // returns the amount of days in the month, considering a leap year.
	 switch (month) 
	 {
	 case 4: case 6: case 9: case 11: return 30; // April, June, September, November
	 case 2: return is_leap_year(year) ? 29 : 28;// February
	 case 1: case 3: case 5: case 7: case 8: case 10: case 12: return 31;
		 // January, March, May, July, August, October, December
	 default: return 0; // Handle invalid months
	 }
 }

 void Date::add(int n)
 { // adds n days to the date
	 set_day_month_year(m_day_number += n);
 }

 void valid_date(unsigned long& days, int& month, int& year)
 { // check if the date is valid by its values
	 if (days > 31)  { month++; days = 1; }
	 if (month > 12) { month = 1; year++; }
 }

ostream& operator<<(ostream& os, const Date& date) 
{
	string dateStr = to_string(date.m_day) + '/' + to_string(date.m_month) + '/' + to_string(date.m_year);
	os << dateStr;
	 return os;
 }

const string Date::GetDate(void) const
{ // return the Date details as string
	return to_string(m_day) + "/" + to_string(m_month) + "/" + to_string(m_year);
}

int Date::operator-(const Date& other) const
{// substruct two dates
	return this->m_day_number - other.m_day_number;
}

bool Date::operator<=(const Date& other) const 
{// compare two dates
	return this->m_day_number <= other.m_day_number;
}

bool Date::operator>=(const Date& other) const 
{// compare two dates
	return this->m_day_number >= other.m_day_number;
}

bool Date::operator<(const Date& other) const
{// compare two dates
	if (m_year < other.m_year) return true;
	if (m_year > other.m_year) return false;
	if (m_month < other.m_month) return true;
	if (m_month > other.m_month) return false;
	return m_day < other.m_day;
}

bool Date::operator==(const Date& other) const
{// compare two dates
	return m_year == other.m_year && m_month == other.m_month && m_day == other.m_day;
}