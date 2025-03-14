#ifndef _USERS_H
#define _USERS_H

#include "Includes.h"
#include "Book.h"

class DSM; // to prevent cyclic includes
struct ResCompare;

const string GetRandomBookCondition(void);

/*=======================================
			Users Class
=========================================*/

class User
{
protected:
	const string m_id; // id of the user
	string m_name; // name of the user
	string m_password; // password of the user
	static int m_MaxCustomerLoans; // the max book loan that each customer can get
public:
	User(const string&, const string&, const string&);
	User(const string&);
	~User() {}
	static int GetMaxCustomerLoans() { return m_MaxCustomerLoans; }
	friend ostream& operator<<(ostream&, const User&);
	const string& GetID(void) const { return m_id; }
	const string& GetPassword(void) const { return m_password; }
	const string& GetName(void) const { return m_name; }
	void SetPassword(const string& password) { m_password = password; }
	void SetName(const string& name) { m_name = name; }
	void HandleSharedCommands(const string&, const string&, DSM&);

protected:
	int SearchBooks(unordered_map<string, Book>::const_iterator&,
		unordered_map<string, Book>::const_iterator&, const string&) const;

	int SearchLoans(unordered_map<string, Loan>::const_iterator&,
		unordered_map<string, Loan>::const_iterator&, const string&) const;

	int SearchLoansHistory(unordered_map<string, LoanHistory>::const_iterator&,
		unordered_map<string, LoanHistory>::const_iterator&, const string&) const;

	int SearchReservations(unordered_map<string, Reservation>::const_iterator&,
		unordered_map<string, Reservation>::const_iterator&, const string&) const;

	int SearchCredMatch(const string&, const string&) const;

	void virtual CallUpdate(DSM&) = 0;
	int UpdatePrint() const;
	virtual bool HandleOtherCommands(const string&, const string&, DSM&) = 0;
	static void ContfromCSV(const string& line, string&, string&, string&);

	template <class T> //prints a header once and the map value with a separator line.
	void PrintMatch(bool& PrintHeader, const string& header,
		typename unordered_map<string, T>::const_iterator& it) const
	{
		if (PrintHeader)
		{
			cout << header << endl << string(header.length(), '-') << endl;
			PrintHeader = false;
		}
		cout << it->second << endl;
		cout << "---------------------------------------------------------------" << endl;
	}

private:
	static void loadMaxBooks(const string&); // load the max book from the file
};

/*=======================================
			Customer Class
=========================================*/

class Customer : public User
{
private:
	unordered_map<string, Loan> m_Loans; // the loan data of the customer
	unordered_multimap<string, LoanHistory> m_LoanHistory; // the loan history data of the customer
	unordered_map<string, Reservation> m_reservations;// the reservations data of the customer
	int m_currloans; // represents the number of books currently loaned by the user.

public:
	Customer(const string& id = "", const string& name = "", const string& = "", const string& path = "");
	Customer(const string& path, int); //another Ctor
	Customer(const Customer&); // CC
	static Customer fromCSV(const string&, const string&);
	bool CheckLoanBook(const string&) const;
	void Move2History(const string&, Date&);
	void CheckHistoryDateRange();
	void DeleteLoan(const string& ISBN);
	const unordered_map<string, Reservation> GetReservations() const { return m_reservations; }
	const unordered_map<string, Loan> GetLoans(void) const { return m_Loans; }
	const unordered_multimap<string, LoanHistory> GetLoanHistory(void) const { return m_LoanHistory; }
	array<stack<string>, 2> DeleteClientData(void);

	int GetCurrLoans() { return m_currloans; }
	void SetCurrLoans(int value) { m_currloans = value; }
	void LoanBookNameISBN(const string&, const unordered_map<string, Book>&);
	void PrintLoanBookNameISBN(const string& filter, const unordered_map<string, Book>& DS);
	int SearchAsCustomer(const string&, const unordered_map<string, Book>&, bool) const;

	// delete the reservation, return an updated iterator
	void RemoveFromRes(const string& ISBN) { m_reservations.erase(ISBN); } 
	
	void ReserveBookISBN(const string&, const unordered_map<string, Book>&);

	unordered_map<string, Loan>::const_iterator 
		GetLoanIteratorBegin() const { return m_Loans.cbegin(); } 

	unordered_map<string, Loan>::const_iterator
		GetLoanIteratorEnd() const { return m_Loans.cend(); }

	unordered_map<string, LoanHistory>::const_iterator 
		GetLoanHistoryIteratorBegin() const { return m_LoanHistory.cbegin(); }
	unordered_map<string, LoanHistory>::const_iterator
		GetLoanHistoryIteratorEnd() const { return m_LoanHistory.cend(); }

	unordered_map<string, Reservation>::const_iterator 
		GetReservationIteratorBegin() const { return m_reservations.cbegin(); }
	unordered_map<string, Reservation>::const_iterator
		GetReservationIteratorEnd() const { return m_reservations.cend(); }

private:
	bool HandleOtherCommands(const string&, const string&, DSM&);
	void CancelBook(const string&, DSM&);
	void WhichFile2Print(const string&) const;
	void ReturnBook(const string&, DSM&);
	bool CheckValidDateRange(string&) const;
	friend ostream& operator<<(ostream&, const Customer&);
	void CallUpdate(DSM&);
	void PrintPriority(const string&);
	void PrintBookCondition(const string&);

	//print the data from the specific file
	template <template <class, class, class...> class Container, typename Val>
	void PrintCustomerFile(const Container<string, Val>& DB) const
	{
		if (DB.empty()) // empty file
			cout << "No data found in this file..." << endl << endl;
		else // data in file
			for (typename Container<string, Val>::const_iterator it = DB.begin(); it != DB.end(); ++it)
				cout << it->second << endl;
	}
};

/*=======================================
			Librarian Class
=========================================*/

class Librarian : public User
{
public:
	Librarian(const string& id = "", const string& name = "", const string & = ""); //Ctor
	Librarian(const Librarian&); // CC
	static Librarian fromCSV(const string&, const string&);
	void CallUpdate(DSM& DS);
private:
	static void SetCustomerLoans(const string&);
	void SearchAsLibrarian(const string&, const unordered_map<string, Librarian>&,
		const unordered_map<string, Customer>&, const unordered_map<string, Book>&) const;
	int SearchLibrarians(unordered_map<string, Librarian>::const_iterator&,
		unordered_map<string, Librarian>::const_iterator&, const string&) const;
	friend ostream& operator<<(ostream&, const Librarian&);
	bool HandleOtherCommands(const string&, const string&, DSM&);
};
#endif // _USERS_H