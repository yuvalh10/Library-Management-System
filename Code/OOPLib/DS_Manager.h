#ifndef _DS_MANAGER_H
#define _DS_MANAGER_H

#include "Includes.h"
#include "Users.h"
#include "Book.h"

// comperator struct to compare between two reservations by priority and dates of reservation
struct ResCompare 
{
    string MY_ISBN;

    ResCompare(const string& ISBN = "") : MY_ISBN(ISBN) {} //Ctor

    bool operator()(Customer* c1, Customer* c2) const // comperator function between two reservations
    {
        unordered_map<string, Reservation> res1 = c1->GetReservations();
        unordered_map<string, Reservation> res2 = c2->GetReservations();
        unordered_map<string, Reservation>::const_iterator it1 = res1.find(MY_ISBN);
        unordered_map<string, Reservation>::const_iterator it2 = res2.find(MY_ISBN);
        // compare based on reservation date
        if (it1->second.GetResDate() == it2->second.GetResDate())
            return it1->second.GetPriority() > it2->second.GetPriority();  // lower number = higher priority

        // if dates are different, compare based on reservation date (older = higher priority)
        return it1->second.GetResDate() > it2->second.GetResDate();
    }
};

class DSM // Data Structure Manager
{
private:
    string m_path; // the path of all the files input/output
    const Customer Null; // only for innitialize the maximum loan value from the text file (its a static variable)
    stack<Book*> m_MulCategories;
    unordered_map<string, Librarian> m_librarians;  // librarians data structure
    unordered_map<string, Customer> m_Customers;  // Customers data structure
    unordered_map<string, Book> m_booksISBN;  // books by ISBN data structure
    unordered_map<string, priority_queue<Customer*, vector<Customer*>, ResCompare>> m_ResQ; // reservations priority queue
    unordered_multimap<string, const Book*> m_booksName; // books by name data structure
    unordered_multimap<string, const Book*> m_booksAuthors; // books by authors data structure
    unordered_multimap<string, const Book*> m_booksPublisher; // books by publisher data structure
    unordered_multimap<string, const Book*> m_booksCategory; // books by category data structure
    unordered_multimap<string, const Book*> m_booksSimLoan; // books by amount of simultanious loan data structure
    unordered_multimap<string, const Book*> m_booksLoanTime; // books by max loan time data structure
    unordered_multimap<string, string> m_PrintLoansnISBN; // for printing the system message for removing automaticly to reservations  
    unordered_multimap<string, string> m_PrintAutoReturn; // for printing the system message for returning the books automaticly

    friend class Library_App;
public:
    ~DSM();
    DSM(const string&); // Ctor
    const unordered_map<string, Book> GetBookISBNDS() const { return m_booksISBN; }
    const unordered_map<string, Customer> GetCustomersDS() const { return m_Customers; }
    const unordered_map<string, Librarian> GetLibrariansDS() const { return m_librarians; }
    const unordered_multimap<string, const Book*> GetBookCategoryDS() const { return m_booksCategory; }
    const unordered_multimap<string, const Book*> GetBookNameDS() const { return m_booksName; }
    const unordered_multimap<string, string> GetPrintLoansISBN() const { return m_PrintLoansnISBN; }
    const string& GetPath() const { return m_path; }
    void SetPath(const string& path) { m_path = path; }
    const string ReturnBookUpdatingDS(const string&, const unordered_map<string, Loan>&);
    void PrintResReturnMSG(const string&);
    friend ostream& operator<<(ostream&, const DSM&);
    void AddBook();
    void RemoveClient(const string&);
    void RemoveCatogory(const string&);
    void CallUpdateBook(const string& );
    void PrintBooks(const string&) const;
    void LoanBook(const string&, const string&);
    string BookNameCount(const string&, int) const;
    string BooksNamesMatches(const string&, int) const;
    void RemoveCustomerFromQueue(const string&, const string&);
    void RemoveBook(const string&);
    void AddClient();
    void AddCategory(const string&);
    void SaveFiles(const string&, unordered_map<string, Customer>::const_iterator) const;
    void UpdateLibrarian(const string&, int);
    void UpdateCustomer(const string&, int);
    void AutomaticReturns();
    void AddNewLibrarian(const string& ID) { ChangeCredentials(ID, m_librarians, 3); } // create new librarian object
    void AvailableFromRes(const string&);

    template <class class_DS> // finds and prints (optional) the book with the ISBN provided
    bool FindBookISBN(const string& ISBN, const unordered_map<string, class_DS>& DS, bool print) const
    {
        typename unordered_map<string, class_DS>::const_iterator it = DS.find(ISBN);
        if (it != DS.end()) // check if the book exists
        {
            if (print)
                it->second.PrintBook(); // prints the book
            return true;
        }
        return false; // no book found
    }

private:
    void ReservationQ_init(const string&);
    void SaveMaxLoanToFile(void) const;
    void UpdateAddingBookDS(const string&, const string&,
        const string&, const string&, const string&, int, int);
    void UpdatingBook(int, const string&, const string&);
    void UpdateRemovingBooksDS(const string&, const string&);
    void PrintsSystemMSG(unordered_multimap<string, string>::iterator&, unordered_multimap<string, string>&,
        const string&, const string&);
    void RemoveBookPtr(unordered_multimap<string, const Book*>&, const Book*);
    void CopyPQ(priority_queue<Customer*, vector<Customer*>, ResCompare>&,vector<Customer*>&) const;
    stack<string> ParseCategories(const string&) const;
    void MultiCategoriesHandle(const string&, const Book&);
    void UpdateCategory(unordered_map<string, Book>::iterator, const string&);
    void LibUpdatesCategory(Book&, const string&);
    void RemoveBookReservationsnLoans(const string&);

    template<typename Container> // writing to file 
    void WriteToFile(const Container& data, ofstream& outFile, const string& headers) const
    {
        outFile << headers << endl;
        for (typename Container::const_iterator it = data.begin(); it != data.end(); ++it)
            outFile << it->second << endl;
    }

    template <class UserType> //updates the credentials for the user (Customer or Librarian)
    void UpdateUsersDB(const string& cred, unordered_map<string, UserType>& result)
    {
        if (cred == "")
            return;
        string ID, name, password;
        stringstream ss(cred);
        getline(ss, ID, ' ');
        getline(ss, name, ' ');
        getline(ss, password);
        if (result.find(ID)->second.GetName() != name) { result.find(ID)->second.SetName(name); }
        if (result.find(ID)->second.GetPassword() != name) { result.find(ID)->second.SetPassword(password); }
    }

    template <class UserType> // change credentials for any user type (Customer or Librarian)
    string ChangeCredentials(const string& ID, unordered_map<string, UserType>& DS, int what2change)
    {
        string name;
        string password;
        while (password.size() < PASSWORD_LENGTH)
        {
            switch (what2change)
            {
            case 1: // change only the name
            {
                password = DS.find(ID)->second.GetPassword();
                cout << "Enter your name: "; getline(cin, name);
                break;
            }
            case 2: // change only the password
            {
                name = DS.find(ID)->second.GetName();
                cout << "Enter your new password: ";  getline(cin, password);
                break;
            }
            case 3: // change both password and name
            {
                cout << "Enter your name: "; getline(cin, name);
                cout << "Enter your new password: ";  getline(cin, password);
                break;
            }
            default: { return ""; }
            }
            if (password.size() < PASSWORD_LENGTH || name.size() < NAME_LENGTH)
                cout << "Password and name length must be at least one character" << endl;
        }

        // check if the user already exists
        typename unordered_map<string, UserType>::iterator it = DS.find(ID);
        if (it != DS.end()) // updates new name and password
        {
            it->second.SetName(name);
            it->second.SetPassword(password);
            cout << endl << "Update successful: Credentials changed." << endl << endl;
        }
        else // for creating new Librarian
            DS.emplace(ID, UserType(ID, name, password));
        return ID + ' ' + name + ' ' + password;
    }

    template <typename T>
    void UpdateBookField(unordered_multimap<string, const Book*>& bookMap,
        T(Book::* getter)() const, void (Book::* setter)(const string&), const string& change, Book& book)
        //  getter function             setter function
    {
        T currentValue = (book.*getter)();
        const string value = ConvertToString(currentValue); // converts to string

        // updating the fields
        unordered_multimap<string, const Book*>::iterator it = bookMap.find(value);
        if (it != bookMap.end())
            bookMap.erase(it);

        // Set the new value using the setter
        (book.*setter)(change);

        // Add the updated book to the map using the new value
        bookMap.emplace(change, &book);
    }

    template <typename T>
    const string ConvertToString(const T& value) { return to_string(value); }  // for int getters

    template <>
    const string ConvertToString<string>(const string& value) { return value; }  // for string getters
};
#endif // _DS_MANAGER_H