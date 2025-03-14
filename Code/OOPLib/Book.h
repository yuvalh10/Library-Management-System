#ifndef _BOOK_H
#define _BOOK_H

#include "Includes.h"
#include "Date.h"
#include "ReadFromFile.h"
class DSM; // to prevent cyclic includes

/*=======================================
			 Book Class
=========================================*/
class Book
{
private:
	const string m_ISBN; // the ID of a book
	string m_name;		 // book name 
	string m_authors;	 // the authors of the book
	string m_publisher;  // book publisher
	string m_category;	 // book category 
	int m_nloans;		 // number of simultaneously loans
	int m_maxloan;		 // maximum loan time in days
public:
	Book(const string& ISBN = "", const string& name = "", const string& authors = "",
		const string& publisher = "", const string& category = "", int nloans = -1, int maxloan = -1);
	friend ostream& operator<<(ostream&, const Book&);
	static Book fromCSV(const string&, const string&);
	const string& GetID(void) const { return m_ISBN; }
	const string& GetName(void) const { return m_name; }
	const string& GetPublisher(void) const { return m_publisher; }
	const string& GetCategory(void) const { return m_category; }
	const string& GetAuthors(void) const { return m_authors; }
	const int& GetMaxLoanTime(void) const { return m_maxloan; }
	const int GetNloans(void) const { return m_nloans; }
	void SetNloans(int value) { m_nloans = value; }
	void PrintBook(void) const;
	void SetName(const string& name) { m_name = name; }
	void SetPublisher(const string& publisher) { m_publisher = publisher; }
	void SetCategory(const string& category) { m_category = category; }
	void SetAuthors(const string& authors) { m_authors = authors; }
	void SetMaxLoanTime(const string& max) { m_maxloan = stoi(max); }
	void SetNloans(const string& Nloans) { m_nloans = stoi(Nloans); }
};

/*=======================================
		   Loan Class
=========================================*/
class Loan
{
protected:
	const string m_ISBN; // the ID of a book
	string m_name; // name of the book
	string m_category; // category of the book
	const Date m_StartLoan; // date of the starting loan
	const Date m_EndLoan; // date of end loan
	string m_BookState; // the state of the book
	int m_DaysLeft; // number of days left to return the book
public:
	Loan(const string& StartLoan, const string& EndLoan, const string& ISBN = "",
		const string& name = "", const string& category = "",
		const string& BookState = "", const string& DaysLeft = "0", const string& _ = "");
	friend ostream& operator<<(ostream&, const Loan&);
	virtual string ToString() const;
	void PrintBook() const { return; }
	const string& GetID(void) const { return m_ISBN; }
	const string& GetName(void) const { return m_name; }
	const string& GetCategory(void) const { return m_category; }
	const string& GetBookState(void) const { return m_BookState; }
	const string GetStartDate(void) const { return m_StartLoan.GetDate(); }
	const string GetEndDate(void) const { return m_EndLoan.GetDate(); }
	const int& GetDaysLeft(void) const { return m_DaysLeft; }
	Loan static fromCSV(const string& line, const string&) { return fromCSVTemp<Loan>(line); }
	template <class T> //template to read details from the loan / loan history files
	static T fromCSVTemp(const string& line) 
	{// extract the content of the book from CSV file
		stringstream ss(line);
		string ISBN, name, category, StartLoan, EndLoan, BookState, DaysLeft,ReturnDate="";

		getline(ss, ISBN, ',');
		getline(ss, name, ',');
		getline(ss, category, ',');
		getline(ss, StartLoan, ',');
		getline(ss, EndLoan, ',');
		getline(ss, BookState, ',');
		getline(ss, DaysLeft, ',');
		return T(StartLoan, EndLoan, ISBN, name, category, BookState, DaysLeft, ReturnDate);
	}
};

/*=======================================
		   LoanHistory Class
=========================================*/

class LoanHistory : public Loan
{
private:
	const Date m_ReturnDate; // the date that the customer return the book 
public:
	LoanHistory(const string&, const string&, const string&, const string&, 
		const string&, const string&, const string&, const string&);
	LoanHistory(const Loan&, const string&);
	LoanHistory static fromCSV(const string& line, const string&) { return fromCSVTemp<LoanHistory>(line); }
	const string GetReturnDate(void) const { return m_ReturnDate.GetDate(); }
	friend ostream& operator<<(ostream& os, const LoanHistory& L);
	string ToString() const;
};

/*=======================================
		   reservation Class
=========================================*/

class Reservation : public Loan
{
private:
	int m_Priority; // the priority of reserve book (that not available)
	const Date m_ResDate; // the date of the reservation
public:
	Reservation(const string&, const string&, const string&, const string&, const string&);
	string ToString() const;
	Reservation static fromCSV(const string& line, const string&) { return fromCSVTemp(line); }
	friend ostream& operator<<(ostream&, Reservation&);
	static Reservation fromCSVTemp(const string& line);
	const string GetResDate(void) const { return m_ResDate.GetDate(); }
	const int& GetPriority(void) const { return m_Priority; }
};
#endif // _BOOK_H