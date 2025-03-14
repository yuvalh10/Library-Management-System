#include "Library_App.h"

#ifdef _WIN32 // works on all Windows versions
void CLS() { system("cls"); }
#else
void CLS() { cout << "\033[2J\033[H"; } // works on Linux/macOS
#endif

Library_App::Library_App(const string& path) : m_ds(new DSM(path)) // initializing the DS
{
	m_ds->AutomaticReturns(); // auto return books from loans past thier date
	Entering2System();
}

void Library_App::Welcome(void) const 
{ // welcome msg
	cout << "=============================================" << endl;
	cout << "Welcome to the Braude College Digital Library" << endl;
	cout << "=============================================" << endl << endl;
}

void Library_App::IDnPass(void) const
{// credentials request as input printing
	cout << "Please insert ID and a Password for entering the system." << endl;
	cout << "To exit type \"Exit\"." << endl << endl;
}

void Library_App::Entering2System(void)
{// first entery to the system
	int attempts = 0; Welcome();
	while (attempts < MAX_ATTEMPTS)
	{
		string ID, password;
		IDnPass();
		cout << "ID: "; getline(cin, ID); // gets the ID
		if (ID == "Exit") { break; }
		cout << "Password: "; getline(cin, password); // gets the password
		string access = CheckCredentials(ID, password);
		if((access !="Librarian" && access != "Customer") || 
			ID.size() < MIN_ID_LENGTH || ID.size() > MAX_ID_LENGTH)  // no match in credentials
		{
			attempts++;
			cout << "Invalid credentials. Attempt " << attempts << " of " << MAX_ATTEMPTS << "." << endl << endl;
			if (attempts >= MAX_ATTEMPTS) // check max reching to attempts 
			{
				cout << "Too many failed attempts. Access denied." << endl;
				return;
			}
		}
		m_ds->AvailableFromRes(ID);// check avaliable books to remove reservations
		if (access == "Librarian") // access as librarian
		{
			PrintLibrarianMenu(ID);
			CLS();
			Welcome();
			attempts = 0;
		}
		else if (access == "Customer") // access as customer
		{
			PrintCustomerMenu(ID);
			CLS();
			Welcome();
			attempts = 0;
		}
	}
}

const string Library_App::CheckCredentials(const string& ID, const string& password)
{ // function that that the credentials if its customer / librarian (new or old)
	const unordered_map<string, Customer> C = m_ds->GetCustomersDS();
	const unordered_map<string, Librarian> L = m_ds->GetLibrariansDS();
	if (ID.find_first_not_of("0123456789") != string::npos || C.find(ID) != C.end() ||
		L.find(ID) != L.end() || password.size() < 1 || ID.size() > MAX_ID_LENGTH ||
		ID.size() < MIN_ID_LENGTH) {} // not valid ID or ID not found
	else if (password == "OOPLib")  // new librarian
	{
		cout << "Hello, new Librarian!" << endl;
		m_ds->AddNewLibrarian(ID); // adds to the DS
		return "Librarian";
	}
	if (CheckUserCredentials(ID, password, C)) // check credentials for customer
		return "Customer";
	else if (CheckUserCredentials(ID, password, L)) // check credentials for librarian
		return "Librarian";
	return "None"; // no match in credentials
}

void Library_App::PrintCustomerMenu(const string& ID)
{// print the menu for the customer to choose operate
	bool flag = true;
	string choice;
	while (choice != "Exit")
	{
		string command, parameter;
		CLS();
		Welcome();
		CustomerHeaderMenu(ID, flag); // flag to see if "system msg" needs to be shown to the user
		SharedMenu();
		UniqCustomerMenu();
		getline(cin, choice);
		ParseChoice(choice, command, parameter); // parsing the input string
		m_ds->m_Customers.find(ID)->second.HandleSharedCommands(command, parameter, *m_ds);// go as librarian
		flag = false;
	}
}

void Library_App::CustomerHeaderMenu(const string& ID, bool flag)
{ // header printing
	cout << "Access granted, Hello Loaner " << m_ds->GetCustomersDS().find(ID)->second.GetName()
		<< "!" << endl;
	if (flag) { m_ds->PrintResReturnMSG(ID); } // check if need to print system messages
	cout << endl << " Customer Menu" << endl;
	cout << "---------------" << endl << endl;
}

void Library_App::LibrarianHeaderMenu(const string& ID)
{// header printing
	cout << "Access granted, Hello Librarian " << m_ds->GetLibrariansDS().find(ID)->second.GetName()
		<< "!" << endl;
	cout << endl << "Librarian Menu" << endl;
	cout << "---------------" << endl << endl;
}

void Library_App::UniqCustomerMenu(void) const
{ // special operations for the customer
	cout << "Loan <ISBN>" << endl;
	cout << "Loan <Name>" << endl;
	cout << "Return <ISBN>" << endl;
	cout << "Return <Name>" << endl;
	cout << "Cancel <ISBN>" << endl;
	cout << "Cancel <Name>" << endl;
	cout << "Print Loans" << endl;
	cout << "Print Reservations" << endl;
	cout << "Print History" << endl;
	cout << "Print_Priority <Priority>" << endl;
	cout << "Print_Book_Condition <Book_Condition>" << endl;
	cout << "Print_Books_Dates_History_Range" << endl;
	cout << "Exit" << endl << endl;
	cout << "Your Choice is: ";
}

void Library_App::SharedMenu(void) const
{ // print shared menu for the librarian and the customer
	cout << "Print <ISBN>" << endl;
	cout << "Print_Books <Category>" << endl;
	cout << "Search <Search by Text>" << endl;
	cout << "Update" << endl;
}

void Library_App::PrintLibrarianMenu(const string& ID)
{// print the menu for the librarian to choose operate
	string choice;
	while (choice != "Exit")
	{
		string command, parameter;
		CLS();
		Welcome();
		LibrarianHeaderMenu(ID);
		SharedMenu();
		UniqLibrarianMenu();
		getline(cin, choice);
		ParseChoice(choice, command, parameter); // parsing librarian choice
		m_ds->m_librarians.find(ID)->second.HandleSharedCommands(command, parameter,*m_ds);
	}
}

void Library_App::UniqLibrarianMenu(void)
{// special operations for the librarian
	cout << "Add Book" << endl;
	cout << "Update_Book <ISBN>" << endl;
	cout << "Remove_Book <ISBN>" << endl;
	cout << "Add Client" << endl;
	cout << "Remove_Client <ID>" << endl;
	cout << "Add_Category <Category>" << endl;
	cout << "Remove_Category <Category>" << endl;
	cout << "Set_Max_Loans <Number of Loans>" << endl;
	cout << "Exit" << endl << endl;
	cout << "Your Choice is: ";
}

void Library_App::ParseChoice(const string& input, string& command, string& parameter) const
{ // parsing the user input for command and parameter
	stringstream ss(input);
	ss >> command;  // extract the command word
	getline(ss, parameter);  // extract the parameter
	if (!parameter.empty() && parameter[0] == ' ')
		parameter = parameter.substr(1);
	else
		parameter = "";
	cout << endl;
}

ostream& operator<<(ostream& os, const Library_App& Library_App)
{
	os << *Library_App.m_ds;
	return os;
}