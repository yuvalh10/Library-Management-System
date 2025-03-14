#ifndef _Library_App_H
#define _Library_App_H

#include "Includes.h"
#include "Book.h"
#include "DS_Manager.h"
#include "Users.h"

class Library_App
{
private:
	DSM* m_ds; // Data structure manager data member
public:
	Library_App(const string& path = "../Data/"); // constructor
	~Library_App() { Deserialization(); delete m_ds; }
	void Deserialization(void) const 
	{ 
		unordered_map<string, Customer>::const_iterator it;
		m_ds->SaveFiles("Outer", it);
	}
private:
	void Welcome(void) const;
	void Entering2System(void);
	void IDnPass(void) const;
	void PrintCustomerMenu(const string&);
	void UniqCustomerMenu(void) const;
	void CustomerHeaderMenu(const string&, bool);
	const string CheckCredentials(const string&, const string&);
	void SharedMenu(void) const;
	void PrintLibrarianMenu(const string&);
	void ParseChoice(const string&, string&, string&) const;
	void LibrarianHeaderMenu(const string&);
	void UniqLibrarianMenu(void);
	
	template <class UserType> // check credentials for any user type (Customer or librarian)
	bool CheckUserCredentials(const string& ID, const string& password,
		const unordered_map<string, UserType>& user_map) const
	{
		/*the typename keyword is used in templates to specify that a dependent name is a type, 
		not a variable or function, helping the compiler understand it correctly*/
		typename unordered_map<string, UserType>::const_iterator user_exist = user_map.find(ID);
		if (user_exist != user_map.end() && user_exist->second.GetPassword() == password)
			return true; // ID and password are correct in the DB
		return false; // ID and password are NOT correct in the DB
	}

	friend ostream& operator<<(ostream&, const Library_App&);
};
#endif // _Library_App_H