#include "Book.h"
#include "DS_Manager.h" // to prevent cyclic includes

/*=======================================
            Book Class
=========================================*/

// Ctor of Book object
Book::Book(const string& ISBN, const string& name, const string& authors, 
           const string& publisher, const string& category, int nloans, int maxloan) :
m_ISBN(ISBN), m_name(name), m_authors(authors), m_publisher(publisher),
m_category(category), m_nloans(nloans), m_maxloan(maxloan){}

Book Book::fromCSV(const string& line, const string&)
{ // parsing the information from the books csv file, and create a book object
    stringstream ss(line);
    string ISBN, name, authors, publisher, category, nloansStr, maxloanStr;
    getline(ss, ISBN, ',');
    getline(ss, name, ',');
    getline(ss, authors, ',');
    getline(ss, publisher, ',');
    getline(ss, category, ',');
    getline(ss, nloansStr, ',');
    getline(ss, maxloanStr);

    int nloans = stoi(nloansStr); // converts from string to int
    int maxloan = stoi(maxloanStr);

    return Book(ISBN, name, authors, publisher, category, nloans, maxloan);
}

void Book::PrintBook(void) const
{ // print the book details 
    cout << "ISBN: " << m_ISBN << endl << endl;
    cout << "Name of the Book: " << m_name << endl << endl;
    cout << "Authors: " << m_authors << endl << endl;
    cout << "Publisher: " << m_publisher << endl << endl;
    cout << "Category: " << m_category << endl << endl;
    cout << "Copies available: " << m_nloans << endl << endl;
    cout << "Maximum Loan Time (Days): " << m_maxloan << endl << endl;
}

ostream& operator<<(ostream& os, const Book& book)
{
    os << book.m_ISBN << "," << book.m_name << "," + book.m_authors << ","
        << book.m_publisher << "," << book.m_category << ","
        << to_string(book.m_nloans) << "," << to_string(book.m_maxloan);
    return os;
}

/*=======================================
           Loan Class
=========================================*/

// Ctor of Loan object
Loan::Loan(const string& StartLoan, const string& EndLoan, const string& ISBN, const string& name,
    const string& category, const string& BookState, const string& DaysLeft,const string&) :
    m_ISBN(ISBN), m_name(name), m_category(category), m_StartLoan(StartLoan), m_EndLoan(EndLoan),
    m_BookState(BookState),
    m_DaysLeft(DaysLeft == "0" ? Date(EndLoan) - Date(StartLoan) : stoi(DaysLeft)) {}

ostream& operator<<(ostream& os, const Loan& L)
{
    os << L.ToString();
    return os;
}

string Loan::ToString() const
{ // specified function for printing Loan 
    return m_ISBN + "," + m_name + "," + m_category + "," + m_StartLoan.GetDate()
        + "," + m_EndLoan.GetDate() + "," + m_BookState + "," + to_string(m_DaysLeft);
}

/*=======================================
            LoanHistory Class
=========================================*/

// Ctor of Loan History object
LoanHistory::LoanHistory(const string& StartLoan, const string& EndLoan, const string& ISBN,
    const string& name, const string& category, const string& BookState,const string& DaysLeft,
    const string& ReturnDate) :
    Loan(StartLoan, EndLoan, ISBN, name, category, BookState, DaysLeft),
    m_ReturnDate(Date().GetDate()) {}

// Copy Ctor of Loan History object
LoanHistory::LoanHistory(const Loan& L, const string& ReturnDate):
    Loan(L.GetStartDate(), L.GetEndDate(), L.GetID(), L.GetName(),L.GetCategory(), L.GetBookState(),
    to_string(L.GetDaysLeft())),m_ReturnDate(ReturnDate) {}

ostream& operator<<(ostream& os, const LoanHistory& L)
{
    os << L.ToString();
    return os;
}

string LoanHistory::ToString() const
{// specified function for printing Loan History
    int loansdays = m_ReturnDate - m_StartLoan; // No need to create Date objects again!
    return m_ISBN + "," + m_name + "," + m_category + "," + m_StartLoan.GetDate()
        + "," + m_ReturnDate.GetDate() + "," + m_BookState + "," + to_string(loansdays);
}

/*=======================================
           reservation Class
=========================================*/

// Ctor of reservation object
Reservation::Reservation(const string& ISBN, const string& name, 
    const string& category, const string& Priority, const string& ResDate) :
    Loan(ResDate, ResDate,ISBN, name, category),
    m_Priority(stoi(Priority)), m_ResDate(Date(ResDate)){}

ostream& operator<<(ostream& os, Reservation& r)
{
    os << r.ToString();
    return os;
}

string Reservation::ToString() const
{// specified function for printing Reservation printing
    return m_ISBN + "," + m_name + "," + m_category + ","
        + m_ResDate.GetDate() + "," + to_string(m_Priority);
}

Reservation Reservation::fromCSVTemp(const string& line) 
{// function to read details from the lReservation file
    stringstream ss(line);
    string ISBN, name, category, ResDate, Priority;

    // parsing each comma-separated value
    getline(ss, ISBN, ',');
    getline(ss, name, ',');
    getline(ss, category, ',');
    getline(ss, ResDate, ',');
    getline(ss, Priority, ',');

    // Return a new Reservation object created with the parsed values
    return Reservation(ISBN, name, category, Priority, ResDate);
}