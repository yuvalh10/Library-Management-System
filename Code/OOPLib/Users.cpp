#include "Users.h"
#include "DS_Manager.h" // to prevent cyclic includes

const string GetRandomBookCondition(void)
{ // randomly select one of the three book conditions
    const string conditions[] = { "New", "Good", "Damaged" };
    return conditions[rand() % 3];
}

/*=======================================
			Users Class
=========================================*/

int User::m_MaxCustomerLoans = 3; // amount of books the customer can borrow

User::User(const string& id, const string& name, const string& password) : // User Ctor
	m_id(id), m_name(name), m_password(password) {}

User::User(const string& path) : m_id(""), m_name(""), m_password("") 
{// another Ctor, called only once to initialize the max loan amount static variable
    loadMaxBooks(path);
}

void User::loadMaxBooks(const string& path)
{ // loan max loan book from file and if now exist it will be 3 as default 
    try
    {
        ifstream file(path + "MaxLoanFile.txt");
        if (file)
        {
            file >> m_MaxCustomerLoans;

            // non opened file and negtive value
            if (file.fail() || m_MaxCustomerLoans < 0)
                m_MaxCustomerLoans = 3; // set default value for starting
        }
        else
            m_MaxCustomerLoans = 3; 
    }
    catch (const exception& e)
    {
        cerr << "Exception in loadMaxBooks: " << e.what() << endl;
        m_MaxCustomerLoans = 3;         
    }
}

void User::ContfromCSV(const string& line, string& id,
    string& name, string& password) 
{// extracting the values from the CSV line
    stringstream ss(line);
    getline(ss, id, ',');
    getline(ss, name, ',');
    getline(ss, password);
}

int User::UpdatePrint(void) const
{// prints messages to update the name, password or both to the customer or librarian
    string choice;
    int attempts = 3; // allow up to 3 attempts

    while (attempts--)
    {
        cout << "Which of the following options would you like to change?" << endl;
        cout << "Name, Password, or Both? ";
        getline(cin, choice);

        if (choice == "Name") { return 1; }
        else if (choice == "Password") { return 2; }
        else if (choice == "Both") { return 3; }
        else // not a valid choice
            cout << endl << "Invalid choice. Please enter 'Name', 'Password', or 'Both'." << endl << endl;
    }
    cout << "Too many invalid attempts. Operation canceled." << endl << endl;;
    return 0; // return 0 if the user fails to enter a valid option
}

void User::HandleSharedCommands(const string& command, const string& parameter, DSM& DS)
{ // handaling shared operations for the librarian and the user
    string _; // only use for returning to the screen menu of user/librarian 
    const unordered_set<string> PrintIgnore = { "History", "Loans", "Reservations" };
    if (command == "Print" && !PrintIgnore.count(parameter)) // Print by ISBN operation
    {// print book info
        if (NOT_VALID_ISBN(parameter)) // check valid ISBN
            cout << "Your ISBN must contain exactly 13 digits." << endl << endl;
        else if (!DS.FindBookISBN(parameter, DS.GetBookISBNDS() ,true)) // found or not found ISBN
            cout << "No book found with ISBN : " + parameter << endl << endl;
    }
    else if (command == "Print_Books") // print books by desired category
        DS.PrintBooks(parameter);
    else if (command == "Update") // update details
        CallUpdate(DS);
    else if (command == "Exit") // exit to the identification screen
        return; 
    else if (HandleOtherCommands(command, parameter, DS)); // other commands, different from customer and librarian
    else // invalid choice
        cout << "Invalid choice. Please select a valid option from the menu." << endl;
    cout << "Press enter key to return to menu...";
    getline(cin, _);
    return;
}

int User::SearchBooks(unordered_map<string, Book>::const_iterator& it, 
    unordered_map<string, Book>::const_iterator& it_end, const string& text) const
{ /* searches for books matching text in various fields, 
    prints matching results, and returns the number of matches.*/
    int matches = 0;
    const string header = "Books Database";
    bool PrintHeader = true;
    for (it; it != it_end; it++)
    { // search for a match in all the DSs
        if (MATCH(it->second.GetID(), text) || MATCH(it->second.GetName(), text) ||
            MATCH(it->second.GetAuthors(), text) || MATCH(it->second.GetCategory(), text) ||
            MATCH(to_string(it->second.GetMaxLoanTime()), text) || MATCH(to_string(it->second.GetNloans()), text)
            || MATCH(it->second.GetPublisher(), text))
        {
            PrintMatch<Book>(PrintHeader, header, it); // prints the matched record
            matches++;
        }
    }
    cout << endl;
    return matches;
}

int User::SearchLoans(unordered_map<string, Loan>::const_iterator& it,
    unordered_map<string, Loan>::const_iterator& it_end, const string& text) const
{ // finds and prints loans matching text across various fields and returns the count of matches.
    int matches = 0;
    const string header = "Loans Database";
    bool PrintHeader = true;
    for (it; it != it_end; it++)
    { // searches for match in the Loans DS
        if (MATCH(it->second.GetID(), text) || MATCH(it->second.GetName(), text) ||
            MATCH(it->second.GetCategory(), text) || MATCH(it->second.GetBookState(), text) ||
            MATCH(it->second.GetStartDate(), text) || MATCH(it->second.GetEndDate(), text) ||
            MATCH(to_string(it->second.GetDaysLeft()), text))
        {
            PrintMatch<Loan>(PrintHeader, header, it); // prints the matched record
            matches++;
        }
    }
    cout << endl;
    return matches;
}

int User::SearchLoansHistory(unordered_map<string, LoanHistory>::const_iterator& it,
    unordered_map<string, LoanHistory>::const_iterator& it_end, const string& text) const
{ // searches and prints loan history records matching text in various fields, returning the number of matches.
    int matches = 0;
    const string header = "Loans History Database";
    bool PrintHeader = true;
    for (it; it != it_end; it++)
    { // search for match in the loan history DS
        if (MATCH(it->second.GetID(), text) || MATCH(it->second.GetName(), text) ||
            MATCH(it->second.GetCategory(), text) || MATCH(it->second.GetBookState(), text) ||
            MATCH(it->second.GetStartDate(), text) || MATCH(it->second.GetEndDate(), text) ||
            MATCH(to_string(it->second.GetDaysLeft()), text))
        {
            PrintMatch<LoanHistory>(PrintHeader, header, it); // prints the matched record
            matches++;
        }
    }
    cout << endl;
    return matches;
}

int User::SearchReservations(unordered_map<string, Reservation>::const_iterator& it,
    unordered_map<string, Reservation>::const_iterator& it_end, const string& text) const
{ // finds and prints reservations matching text in various fields, returning the number of matches.
    int matches = 0;
    const string header = "Reservations Database";
    bool PrintHeader = true;
    for (it; it != it_end; it++)
    {// search for match in the reservations DS
        if (MATCH(it->second.GetID(), text) || MATCH(it->second.GetName(), text) ||
            MATCH(it->second.GetCategory(), text) || MATCH(it->second.GetResDate(), text) ||
            MATCH(to_string(it->second.GetPriority()), text))
        {
            PrintMatch<Reservation>(PrintHeader, header, it); // prints the matched record
            matches++;
        }
    }
    cout << endl;
    return matches;
}

int User::SearchCredMatch(const string& user, const string& text) const
{// checks if the user's credentials match text, prints the details if found, and returns 1 for a match or 0 otherwise.
    const string header = user + m_id + " credentials matches:";
    // search for match in user credentials
    if (MATCH(m_id, text) || MATCH(m_name, text) || MATCH(m_password,text))
    {
        cout << header << endl;
        cout << string(header.size(), '-') << endl;
        cout << *this << endl << endl; // prints if match
        return 1;
    }
    return 0;
}

ostream& operator<<(ostream& os, const User& user)
{
    os << user.m_id << "," << user.m_name << "," << user.m_password << "," << user.m_MaxCustomerLoans;
    return os;
}

/*=======================================
			Customer Class
=========================================*/

Customer::Customer(const string& id, const string& name, const string& password, const string& path) // Customer Ctor
    : User(id, name, password), // initializing credentials  
    m_Loans(ReadFromFile<Loan>(m_id + "_loans.csv", path)), // initializing loan info
    m_LoanHistory(ReadFromFile<LoanHistory, unordered_multimap>(m_id + "_loansHistory.csv", path)), // initializing loan history info
    m_reservations(ReadFromFile<Reservation>(m_id + "_reservations.csv", path)) // initializing reservations info
{
     m_Loans.empty() ? m_currloans = 0 : m_currloans = m_Loans.size(); // initializing amount of avaliable loans
}

Customer::Customer(const string& path, int) // Customer Ctor
    : User(path),m_currloans(0) {} // initializing credentials 
    
Customer::Customer(const Customer& other) // CC
    : User(other.m_id, other.m_name, other.m_password),
    m_LoanHistory(other.m_LoanHistory), m_Loans(other.m_Loans),
    m_reservations(other.m_reservations), m_currloans(other.m_currloans) {}

Customer Customer::fromCSV(const string& line, const string& path)
{ // creates a Customer object from every line in the CSV file
    string id, name, password;
    ContfromCSV(line, id, name, password);
    return Customer(id, name, password,path);
}

bool Customer::HandleOtherCommands(const string& command, const string& parameter, DSM& DS)
{// processes customer commands for loans, returns, cancellations, searches, prints, returning true if handled
    if (command == "Loan") // Loan by ISBN or by name
    {
        if (ISBN_OR_NAME(parameter)) // check for valid ISBN or name
            DS.LoanBook(parameter, m_id);
        else
            cout << "Your ISBN must contain exactly 13 digits." << endl << endl;
        return true;
    }
    else if (command == "Return") // Returns a book from Loans by name/ISBNs
    {
        if (ISBN_OR_NAME(parameter)) // check for valid ISBN or name
            ReturnBook(parameter, DS);
        else
            cout << "Your ISBN must contain exactly 13 digits." << endl << endl;
        return true;
    }
    else if (command == "Cancel") // cancel book from reserves by name/ISBN
    {
        if (ISBN_OR_NAME(parameter)) // check for valid ISBN or name
            CancelBook(parameter, DS);
        else
            cout << "Your ISBN must contain exactly 13 digits." << endl << endl;
        return true;
    }
    else if (command == "Search") // search as customer
    {
        SearchAsCustomer(parameter,DS.GetBookISBNDS(),true);
        return true;
    }
    else if (command == "Print" && // prints loans, history and reservations
        (parameter == "Loans" || parameter == "Reservations" || parameter == "History"))
    {
        WhichFile2Print(parameter); // print desired file content
        return true;
    }
    else if (command == "Print_Priority")
    {
        PrintPriority(parameter); // print by reservations by priority
        return true;
    }
    else if(command == "Print_Book_Condition") // print book condition in loans file
    {
        PrintBookCondition(parameter);
        return true;
    }
    else if (command == "Print_Books_Dates_History_Range") // prints loans history, filtered by date range
    {
        CheckHistoryDateRange();
        return true;
    }
    return false;
}

void Customer::CheckHistoryDateRange()
{ // prompts the user for a loan history date range, validates it, and prints matching loan records.
    cout << "Please check you History loan range in format dd/mm/yyyy - dd/mm/yyyy: ";
    string historyrange;
    getline(cin, historyrange);
    if (historyrange.size() != DATE_FORMAT) // check for invalid date format
    {
        cout << "invalid date format" << endl;
        return;
    }
    string startdatestr = historyrange.substr(0, 10); // parsing into 2 dates
    string enddatestr = historyrange.substr(13,10);
    if (!CheckValidDateRange(startdatestr) || !CheckValidDateRange(enddatestr)) // check validation of each parsed date
        return;
    Date startDate(startdatestr);
    Date endDate(enddatestr);
    if (endDate - startDate < 0) // check if end date is before start date
    {
        cout << "your end date is before your start date" << endl;
        return;
    }
    const string header = "Your Books History loans from dates: " + startdatestr + " to: " + enddatestr;
    cout << endl <<header << endl;
    cout << string(header.length(), '-') << endl << endl;

    int counter = 0;
    unordered_map<string, LoanHistory>::iterator it;
    for (it= m_LoanHistory.begin(); it != m_LoanHistory.end(); ++it) // search between the dates
    {
        const LoanHistory& loanHistory = it->second;  
        Date returnDate(loanHistory.GetReturnDate()); // gets the return date 

        if (returnDate >= startDate && returnDate <= endDate) // check if between the given dates range
        {
            cout << loanHistory << endl;
            counter++;
        }
    }
    cout << endl;
    if (!counter)
        cout << "Could not find any books in your history loan: " << historyrange << endl << endl;
}

bool Customer::CheckValidDateRange(string& historyrange) const
{ // validates the date range format and checks if the day, month, and year are valid.
    // expected format: dd/mm/yyyy (length 10)
    if (historyrange.size() != DATE_LENGTH || historyrange[2] != '/' || historyrange[5] != '/')
    {
        cout << "Invalid date format." << endl;
        return false;
    }

    // check if all characters are digits (except format characters)
    if (historyrange.find_first_not_of("0123456789/") != string::npos)
    {
        cout << "Invalid date format." << endl;
        return false;
    }
    // extract day, month, year
    int day = stoi(historyrange.substr(0, 2));
    int month = stoi(historyrange.substr(3, 2));
    int year = stoi(historyrange.substr(6, 4));

    // check valid ranges
    Date check(day,month,year);
    if (month > 12 || month < 1 || day > num_days_month(month, year) || day < 1)
    {
        cout << "Invalid date range format." << endl;
        return false;
    }
    return true;
}

void Customer::PrintBookCondition(const string& condition)
{/* prints loans with the specified book condition(New, Good, or Damaged),
    or an error message if no match is found.*/
    if (condition != "Damaged" && condition != "New" && condition != "Good") // not a valid book condition
    {
        cout << "Invalid book condition" << endl << endl;
        return;
    }
    const string header = "Your Books loans in condition: " + condition;
    unordered_map<string, Loan>::iterator it;
    cout << header << endl;
    cout << string(header.length(), '-') << endl << endl;
    int counter = 0;
    for (it = m_Loans.begin(); it != m_Loans.end(); ++it) // search for any match in loan of the book cond
    {
        if (it->second.GetBookState() == condition)
        {
            cout << it->second << endl;
            counter++;
        }   
    }
    cout << endl;
    if (!counter)
        cout << "Could not find any books in your loans with the condition: " << condition << endl << endl;
}

void Customer::PrintPriority(const string& priority)
{// prints reservations with the specified priority (1-5), or an error message if the priority is invalid.
    if (VALID_PRIOTITY(priority)) // check fo valid priority
    {
        cout << "Invalid Priority number" << endl;
        return;
    }
    const string header = "Your reservation for the priority: " + priority;
    unordered_map<string, Reservation>::iterator it;
    cout << header << endl;
    cout << string(header.length(), '-') << endl << endl;
    for (it = m_reservations.begin(); it != m_reservations.end(); ++it)
        if (it->second.GetPriority() == stoi(priority)) // check if reservation matches the current priority
            cout << it->second << endl;
    cout << endl;
}

bool Customer::CheckLoanBook(const string& ISBN) const
{// verifies if a book can be loaned based on existing loans, reservations, or max limit, 
  // returning false if any condition fails.
    unordered_map<string, Loan>::const_iterator itl = m_Loans.find(ISBN);
    unordered_map<string, Reservation>::const_iterator itr = m_reservations.find(ISBN);
    if (ISBN == "")  // check if ISBN was found
    {
        cout << "This book does not exist in the library." << endl << endl;
        return false;
    }
    else if (itl != m_Loans.end()) // exists in the Loans DB
    {
        cout << "You already have this book in your loan list!" << endl;
        cout << "Each borrower is allowed only one copy." << endl << endl;
        return false;
    }
    else if (itr != m_reservations.end()) // exists in the Reservations DB
    {
        cout << "You already have this book in your reservations list" << endl << endl;
        return false;
    }
    return true;
}

void Customer::PrintLoanBookNameISBN(const string& ISBN, const unordered_map<string, Book>& DS)
{ // loans the book and prints a msg
    LoanBookNameISBN(ISBN, DS); // loans a book by the ISBN
    cout << "Found a copy to borrow!" << endl;
    cout << "Book successfully added to your loan list!" << endl << endl;
}

void Customer::LoanBookNameISBN(const string& ISBNN, const unordered_map<string, Book>& DS)
{// loans a book after a copy has been found and all other conditions are passed
    string name = DS.find(ISBNN)->second.GetName(); // gets the book name
    string ISBN;
    int DaysLeft = 0;
    Date StartLoanTime, EndLoanTime;
    EndLoanTime.add(DS.find(ISBNN)->second.GetMaxLoanTime()); // sets the end loan time
    string category, BookState;
    ISBN = DS.find(ISBNN)->second.GetID();
    category = DS.find(ISBNN)->second.GetCategory(); // gets the book category
    BookState = GetRandomBookCondition(); // gets the book condition
    DaysLeft = DS.find(ISBNN)->second.GetMaxLoanTime(); // gets the time to borrow the book
    m_currloans++; // increment the amount of current loans
    m_Loans.emplace(ISBNN, Loan(StartLoanTime.GetDate(), EndLoanTime.GetDate(), ISBN, name, category,
        BookState, to_string(DaysLeft))); // put the loan object in the loan DB
}

void Customer::ReserveBookISBN(const string& ISBN, const unordered_map<string, Book>& Book_DS)
{ // adds a book to the customer's reservation list with a valid priority and updates the reservation queue.
    cout << "Adding your desired book to your reservations list:" << endl << endl;
    string priority, name, category;
    Date now;
    name = Book_DS.find(ISBN)->second.GetName();
    category = Book_DS.find(ISBN)->second.GetCategory();
    do // gets the priority of the reservation
    {
        cout << "Enter the priority of your loan (1-5): ";
        getline(cin, priority);
        // check if the input is a single-digit number between '1' and '5'
        bool isValid = (priority.size() == 1 && priority[0] >= '1' && priority[0] <= '5');
        if (!isValid) // not a valid priority
            cout << "Invalid input. Please enter a number between 1 and 5." << endl;
    } while (priority.size() != 1 || priority[0] < '1' || priority[0] > '5');
    m_reservations.emplace(ISBN, Reservation(ISBN, name, category, priority, now.GetDate())); // adds to reservations
}

void Customer::CancelBook(const string& filter, DSM& DS)
// cancel a book from the reservation DB
{
    int matches = 0;
    string ISBN, name = filter;
    
    unordered_map<string, Book> ISBN_DS = DS.GetBookISBNDS();
    unordered_multimap<string, const Book*> BooksNamesDS = DS.GetBookNameDS();

    unordered_map<string, Book>::const_iterator itISBN = ISBN_DS.find(filter);
    unordered_multimap<string, const Book*>::const_iterator itName = BooksNamesDS.find(filter);

    if (itISBN == ISBN_DS.end() && itName == BooksNamesDS.end()) // check existence in reservations list
    {
        cout << "Your desired book does not exist in your reservations list!" << endl << endl;
        return;
    }
    if (itISBN != ISBN_DS.end()) // extract the ISBN
        ISBN = filter;
    else if (itName != BooksNamesDS.end()) // extract the ISBN if by name
    {
        ISBN = DS.BookNameCount(filter, matches);
        name = BooksNamesDS.find(filter)->second->GetName();
    }
    bool found = DS.FindBookISBN(ISBN, m_reservations, false); // check if the book exists in the reservation DB 
    if(found) // delete from DB
    { 
        unordered_map<string, Reservation>::iterator it = m_reservations.find(ISBN);
        if (it != m_reservations.end())
            RemoveFromRes(it->second.GetID()); // removes the reservation
        DS.RemoveCustomerFromQueue(ISBN, m_id); // updating the reservations queue DB
        if (name != "")
            cout << "The book " << "\"" << name << "\"";
        else
            cout << "Book with ISBN: " << ISBN;
        cout << " has been successfully canceled." << endl << endl;
    }
    else
        cout << "No book found with the name: " << "\"" << name << "\"" << endl << endl;
}

void Customer::ReturnBook(const string& filter, DSM& DS)
{ // returns a book by either its ISBN or name, updates loan history, and adjusts the book's loan count.
    const string ISBN = DS.ReturnBookUpdatingDS(filter,m_Loans); // returns the book and updating DS
    if (ISBN == "")
        return;
    Date returnDate;
    Move2History(ISBN, returnDate); // moves the book to the history
}

void Customer::Move2History(const string& ISBN, Date& returnDate)
{
    m_LoanHistory.emplace(ISBN, LoanHistory(m_Loans.find(ISBN)->second, returnDate.GetDate())); // insert to loan history
    DeleteLoan(ISBN); // delete the loan
}

void Customer::DeleteLoan(const string& ISBN)
{ // delete the loan if found such by the ISBN
    if (m_Loans.find(ISBN) != m_Loans.end())
    {
        m_Loans.erase(ISBN);
        m_currloans--;
    }
}

void Customer::WhichFile2Print(const string& parameter) const
{// check what file to print
    if (parameter == "Loans") // print loans
        PrintCustomerFile<unordered_map>(m_Loans);
    else if (parameter == "Reservations") // print reservations
        PrintCustomerFile<unordered_map>(m_reservations);
    else if (parameter == "History") // print history
        PrintCustomerFile<unordered_multimap>(m_LoanHistory);
}

void Customer::CallUpdate(DSM& DS)
{// calls updating CUSTOMER credentidals function
    DS.UpdateCustomer(GetID(), UpdatePrint());
}

int Customer::SearchAsCustomer(const string& text, const unordered_map<string, Book>& ISBN_DS, bool printdata) const
{/* searches through books, loans, loan historiesand reservations
    for a given text and prints results, returning the total match count.*/
    int matches = SearchCredMatch("Customer ", text); // match own creddentials

    if (printdata)
    {
        unordered_map<string, Book>::const_iterator it_books_start = ISBN_DS.begin();
        unordered_map<string, Book>::const_iterator it_books_end = ISBN_DS.end();
        matches += SearchBooks(it_books_start, it_books_end, text); // print matched book record
    }

    unordered_map<string, Loan>::const_iterator it_loan_start = m_Loans.begin();
    unordered_map<string, Loan>::const_iterator it_loan_end = m_Loans.end();
    matches += SearchLoans(it_loan_start, it_loan_end, text); // print matched loan record
    
    unordered_map<string, LoanHistory>::const_iterator it_LH_start = m_LoanHistory.begin();
    unordered_map<string, LoanHistory>::const_iterator it_LH_end = m_LoanHistory.end();
    matches += SearchLoansHistory(it_LH_start, it_LH_end, text); // print matched loan history record

    unordered_map<string, Reservation>::const_iterator it_Res_start = m_reservations.begin();
    unordered_map<string, Reservation>::const_iterator it_Res_end = m_reservations.end();
    matches += SearchReservations(it_Res_start, it_Res_end, text); // print matched reservation record

    if (printdata) // prints total matches
        cout << endl << "A total of " << matches << " matches were found for your search." << endl << endl;
    
    return matches;
}

array<stack<string>, 2> Customer::DeleteClientData(void)
{
    array<stack<string>, 2> Updates;
    unordered_map<string, Loan>::iterator itl;
    unordered_map<string, Reservation>::iterator itr;
    for (itl = m_Loans.begin(); itl != m_Loans.end(); itl++)
        Updates[0].push(itl->second.GetID());
    for (itr = m_reservations.begin(); itr != m_reservations.end(); itr++)
        Updates[1].push(itr->second.GetID());
    m_Loans.clear();
    m_reservations.clear();
    m_currloans = 0;
    return Updates;
}

ostream& operator<<(ostream& os, const Customer& cus)
{
    os << cus.m_id << "," << cus.m_name << "," << cus.m_password;
    return os;
}

/*=======================================
			Librarian Class
=========================================*/

Librarian::Librarian(const string& id, const string& name, const string& password) // Librarian Ctor 
	: User(id, name, password) {} 

Librarian::Librarian(const Librarian& other) // Librarian CC 
    : User(other.m_id, other.m_name, other.m_password) {}

Librarian Librarian::fromCSV(const string& line, const string&)
{ // creates a librarian object from every line in the CSV
    string id,name, password;
    ContfromCSV(line, id, name, password);
    return Librarian(id, name, password);
}

bool Librarian::HandleOtherCommands(const string& command, const string& parameter, DSM& DS)
{// processes librarian commands like adding/removing books, clients, categories, updating books, or setting loan limits,
    // forwarding the request to the appropriate method , returning true if handled.
    if (command == "Add") 
    {
        if (parameter == "Book") // add a new book
            DS.AddBook();
        else if (parameter == "Client") // add new client
            DS.AddClient();
        return true;
    }
    else if (command == "Add_Category") // add new category
    {
        DS.AddCategory(parameter);
        return true;
    }
    else if (command == "Remove_Book") // remove existing book
    {
        DS.RemoveBook(parameter);
        return true;
    }
    else if (command == "Search") // search as libbrarian in all DB
    {
        SearchAsLibrarian(parameter, DS.GetLibrariansDS(), DS.GetCustomersDS(), DS.GetBookISBNDS());
        return true;
    }
    else if (command == "Remove_Client") // remove existing client
    {
        DS.RemoveClient(parameter);
        return true;
    }
    else if (command == "Remove_Category") // remove existing category
    {
        DS.RemoveCatogory(parameter);
        return true;
    }
    else if (command == "Update_Book") // updating existing book parameters
    {
        DS.CallUpdateBook(parameter);
        return true;
    }
    else if (command == "Set_Max_Loans") // set different maximum loans per customer (defualt = 3)
    {
        SetCustomerLoans(parameter);
        return true;
    }
    
    return false;
}

void Librarian::SetCustomerLoans(const string& maxloan) 
{ // sets the max book the customer can loan
    int ivalue = -1;
    string value = maxloan;

    while (ivalue < 0) 
    {
        try
        {
            ivalue = stoi(value); 

            if (ivalue >= 0) // valid input 0 or greater
            {
                m_MaxCustomerLoans = ivalue;
                //SaveMaxLoan();
                cout << "Maximum loan limit has been successfully updated" << endl << endl;
                return; 
            }
        }
        catch (exception& e)
        {
            cout << "Invalid input! Please enter a valid number." << endl;
            cout << "Error: " << e.what() << endl;
        }
        cout << "Please enter the new maximum number of loans allowed for borrowers: ";
        getline(cin, value); // get new input from user
    }
}

void Librarian::CallUpdate(DSM& DS)
{// change LIBRARIAN credentials in DB
    DS.UpdateLibrarian(GetID(), UpdatePrint());
}

void Librarian::SearchAsLibrarian(const string& text, const unordered_map<string, Librarian>& LibDS,
    const unordered_map<string, Customer>& CusDS, const unordered_map<string, Book>& ISBN_DS) const
{// searches for matches in librarians, books, and customers based on the given text,
    // then prints the total number of matches found.
    int matches = SearchCredMatch("Librarian ", text); // search if credentials match for the current librarian
    
    unordered_map<string, Librarian>::const_iterator it_lib_start = LibDS.begin();
    unordered_map<string, Librarian>::const_iterator it_lib_end = LibDS.end();
    matches += SearchLibrarians(it_lib_start, it_lib_end,text); // search in librarians DS

    unordered_map<string, Book>::const_iterator it_books_start = ISBN_DS.begin();
    unordered_map<string, Book>::const_iterator it_books_end = ISBN_DS.end();
    matches += SearchBooks(it_books_start, it_books_end, text); // search in books DS

    unordered_map<string, Customer>::const_iterator it = CusDS.begin();
    unordered_map<string, Customer>::const_iterator it_cus_end = CusDS.end();

    for (it; it != it_cus_end; it++) // iterates through all customers in DS
        matches += it->second.SearchAsCustomer(text, ISBN_DS,false); // rest of the search as a customer

    cout << endl << "A total of " << matches << " matches were found for your search." << endl << endl;
}

int Librarian::SearchLibrarians(unordered_map<string, Librarian>::const_iterator& it,
    unordered_map<string, Librarian>::const_iterator& it_end, const string& text) const
{// searches for matching librarians (excluding the current one) and returns the match count.
    int matches = 0;
    const string header = "Librarian Database";
    bool PrintHeader = true;
    for (it; it != it_end; it++) // check if any match
    {
        if ((MATCH(it->second.GetID(), text) && it->second.GetID() != m_id) || 
            (MATCH(it->second.GetName(), text) && it->second.GetName() != m_name) || 
            (MATCH(it->second.GetPassword(), text) && it->second.GetPassword() != m_password))
        {
            PrintMatch<Librarian>(PrintHeader, header, it); // print the record
            matches++;
        }
    }
    cout << endl;
    return matches;
}

ostream& operator<<(ostream& os, const Librarian& lib)
{
    os << lib.m_id << "," << lib.m_name << "," << lib.m_password;
    return os;
}