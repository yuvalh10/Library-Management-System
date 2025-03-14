#include "DS_Manager.h"

// creates an unordered map for Customers, Librarians and Books 
DSM::DSM(const string& path) :
    m_path(path), Null(path,0),
    m_booksISBN(ReadFromFile<Book>("Books.csv", m_path)),
    m_librarians(ReadFromFile<Librarian>("Librarians.csv", m_path)),
    m_Customers(ReadFromFile<Customer>("Loaners.csv", m_path))
{
    unordered_map<string, Book>::const_iterator it;
    for (it = m_booksISBN.begin(); it != m_booksISBN.end(); it++) 
    {// insertes the books by specific filter: 
        m_booksName.emplace(it->second.GetName(), &it->second); // books by name

        m_booksPublisher.emplace(it->second.GetPublisher(), &it->second); // books by publisher

        m_booksSimLoan.emplace(to_string(it->second.GetNloans()), &it->second); // books by amount of simultaneous loan

        m_booksLoanTime.emplace(to_string(it->second.GetMaxLoanTime()), &it->second); // books by max loan time

        m_booksAuthors.emplace(it->second.GetAuthors(), &it->second); // books by authors
        
        string categories = it->second.GetCategory();
        if (categories.find('+') != string::npos) // contains multiple categories
            MultiCategoriesHandle(categories, it->second);
        else
            m_booksCategory.emplace(categories, &it->second); // books by category

        if (it->second.GetNloans() == 0) // create reservation priority queue for unavaliabe books
            ReservationQ_init(it->second.GetID());
    }
}

void DSM::ReservationQ_init(const string& ISBN)
{ // initializes a priority queue for a given ISBN and populates it with customers who have reserved that book.
    unordered_map<string, Customer>::iterator it;
    if (m_ResQ.find(ISBN) == m_ResQ.end())  // ensure priority_queue exists for that ISBN
        m_ResQ.emplace(ISBN,priority_queue<Customer*, vector<Customer*>, ResCompare>(ResCompare(ISBN)));

    for (it = m_Customers.begin(); it != m_Customers.end(); it++) // populate the priority queue
    {
        unordered_map <string, Reservation> res = it->second.GetReservations();
        if (res.find(ISBN) != res.end())
            m_ResQ[ISBN].push(&it->second);
    }
}

DSM::~DSM()
{ // dtor clears all data structures in DSM.
    m_librarians.clear();
    m_Customers.clear();
    m_booksISBN.clear();
    m_booksName.clear();
    m_booksAuthors.clear();
    m_booksPublisher.clear();
    m_booksCategory.clear();
    m_booksSimLoan.clear();
    m_booksLoanTime.clear();

    while (!m_MulCategories.empty()) // empties allocated pointers
    {
        delete m_MulCategories.top();
        m_MulCategories.pop(); // remove pointer from stack
    }
}

const string DSM::ReturnBookUpdatingDS(const string& filter, const unordered_map<string, Loan>& loans)
{ // returns a book by either its ISBN or name, updates loan history, and adjusts the book's loan count.
    int matches = 0;
    string ISBN, name;
    unordered_map<string, Book>::const_iterator itISBN = m_booksISBN.find(filter);
    unordered_multimap<string, const Book*>::const_iterator itName = m_booksName.find(filter);
    if (itISBN != m_booksISBN.end()) { ISBN = m_booksISBN.find(filter)->second.GetID(); } // gets the ISBN
    else if (itName != m_booksName.end()) // if by name, gets the ISBN 
    {
        name = m_booksName.find(filter)->second->GetName();
        ISBN = BookNameCount(filter, matches);
    }
    if (!FindBookISBN<Loan>(ISBN, loans, false)) // check if already in loan list
    {
        cout << "No book found in your loan list." << endl << endl;
        return "";
    }
    int copies = m_booksISBN.find(ISBN)->second.GetNloans();
    m_booksISBN.find(ISBN)->second.SetNloans(copies + 1); // update the copies in the library
    if (name != "")
        cout << "The book " << "\"" << name << "\"";
    else
        cout << "The book with ISBN: " << ISBN;
    cout << " has been successfully returned to the library!" << endl << endl;
    return ISBN;
}

void DSM::PrintBooks(const string& category) const
{ // get the range of books for a specific category - first: first elem, last: one past the last elem.
    unordered_multimap<string, const Book*>::const_iterator first = m_booksCategory.equal_range(category).first;
    unordered_multimap<string, const Book*>::const_iterator last = m_booksCategory.equal_range(category).second;
    unordered_multimap<string, const Book*>::const_iterator it = first;
    int counter = 0, total = 0;;
    if (m_booksCategory.find(category) == last) // check if category exists
    {
        cout << "Category didnt exist in the library." << endl << endl;
        return;
    }
    string header = "Books in Category \"" + category + "\"";
    string separator = string(header.length(), '-');
    cout << header << endl << separator << endl << endl;
    for (it = first; it != last; it++) // prints the matched books by category
    {
        string input;
        if (it->second != nullptr)
        {
            it->second->PrintBook(); // prints the book
            cout << endl;
            counter++; total++;
            cout << "---------------------------------------------" << endl << endl;
        }
        if (counter / MORE == 1) // hold on for user decision
        {
            while (input != "More" && input != "Return")
            {
                cout << "Type \"More\" to display the next 10 books in the "
                    << category << " category." << endl;
                cout << "Type \"Return\" to go back to the menu." << endl;
                cout << "Type here: ";
                getline(cin, input); cout << endl;
            }
            if (input == "Return") { return; }
            counter = 0;
        }
    }
    cout << endl << "No more books in the \"" << category << "\" category. There are a total of "
        << total << " books in this category." << endl << endl;
}

void DSM::SaveMaxLoanToFile() const
{// save the max loan value to a file
    ofstream outFile(m_path + "MaxLoanFile.txt");
    if (outFile)
        outFile << User::GetMaxCustomerLoans() << endl;
    else
        cerr << "Error opening MaxLoanFile.txt for writing." << endl;
    outFile.close();
}

ostream& operator<<(ostream& os, const DSM& dsm) 
{
    unordered_map<string, Librarian>::const_iterator lib_it;
    unordered_map<string, Customer>::const_iterator cus_it;
    unordered_map<string, Book>::const_iterator isbn_it;
    unordered_multimap<string, string>::const_iterator resprint_it;
    unordered_map<string, priority_queue<Customer*, vector<Customer*>, ResCompare>>::const_iterator resq_it;
    unordered_multimap<string, string>::const_iterator AutoReturn_it;

    // Print files path
    os << "---Files path---" << endl;
    os << dsm.m_path << endl;

    // Print Librarians
    os << "---Librarians---" << endl;    
    for (lib_it = dsm.m_librarians.begin(); lib_it != dsm.m_librarians.end(); lib_it++)
        os << lib_it->first << ": " << lib_it->second << endl;

    // Print Customers
    os << endl << "---Customers---" << endl;
    for (cus_it = dsm.m_Customers.begin(); cus_it != dsm.m_Customers.end(); cus_it++)
        os << cus_it->first << ": " << cus_it->second << endl;

    // Print Books by ISBN
    os << endl << "---Books By ISBN---" << endl;
    for (isbn_it = dsm.m_booksISBN.begin(); isbn_it != dsm.m_booksISBN.end(); isbn_it++)
        os << isbn_it->first << ": " << isbn_it->second << endl;

    // Print PrintLoansnISBN 
    os << endl << "---Print Loans ISBN---" << endl;
    for (resprint_it = dsm.m_PrintLoansnISBN.begin(); resprint_it != dsm.m_PrintLoansnISBN.end(); resprint_it++)
        os << resprint_it->first << ": " << resprint_it->second << endl;

    // Print PrintAutoReturn 
    os << endl << "---Print Loans ISBN---" << endl;
    for (AutoReturn_it = dsm.m_PrintAutoReturn.begin(); AutoReturn_it != dsm.m_PrintAutoReturn.end(); AutoReturn_it++)
        os << AutoReturn_it->first << ": " << AutoReturn_it->second << endl;

    // Print Reservations Queue (m_ResQ)
    os << endl << "---Reservations Queue---" << endl;
    for (resq_it = dsm.m_ResQ.begin(); resq_it != dsm.m_ResQ.end(); resq_it++) 
    {
        os << resq_it->first << ": ";
        
        if (!resq_it->second.empty()) // Print the priority queue (top element)
            os << *(resq_it->second.top()) << endl;
        else
            os << "empty\n";
    }
    return os;
}

void DSM::SaveFiles(const string& type, unordered_map<string, Customer>::const_iterator it_c) const
{//save the file and its data's (inner and outer files)
    unordered_map<string, string> filenames;
    string headers = "";
    if (type == "Outer") // Saving Librarians, Customers, and Books
    {
        filenames["Librarians"] = "Librarians.csv";
        filenames["Customers"] = "Loaners.csv";
        filenames["Books"] = "Books.csv";
    }
    else if (type == "Inner") // Saving Loans, LoansHistory, and Reservations for a customer
    {
        filenames["Loans"] = "_loans.csv";
        filenames["LoansHistory"] = "_loansHistory.csv";
        filenames["Reservations"] = "_reservations.csv";
    }

    unordered_map<string, string>::iterator it;
    for (it = filenames.begin(); it != filenames.end(); ++it)
    {
        try
        {
            string filename = (type == "Outer") ? (m_path + it->second) : (m_path + it_c->second.GetID() + it->second);
            ofstream outFile(filename);
            if (!outFile)
                throw runtime_error("Failed to open file: " + filename);

            if (type == "Outer")
            {
                if (it->first == "Librarians") // saving librarians file
                {
                    headers = "Librarian ID,Librarian Name,Librarian Password";
                    WriteToFile(m_librarians, outFile, headers);
                }
                else if (it->first == "Customers") // saving customers file
                {
                    unordered_map<string, Customer>::const_iterator runner;
                    headers = "Loaner ID,Loaner Name,Loaner Password";
                    WriteToFile(m_Customers, outFile, headers);
                    for(runner = m_Customers.cbegin(); runner != m_Customers.cend(); runner++)
                        SaveFiles("Inner", runner); // another call for the inner files of the customer
                }
                else if (it->first == "Books") // saving books file
                {
                    headers = "ISBN,Book Name,Authors,Publisher,Category,Maximum Simultaneous Loans, Maximum Loan Time";
                    WriteToFile(m_booksISBN, outFile, headers);
                }
            }
            else if (type == "Inner")
            {
                if (it->first == "Loans") // saving Loans file
                {
                    headers = "ISBN,Book Name,Category,Loan Start Date,Loan End Date,Book Condition,Days left until deadline";
                    WriteToFile(it_c->second.GetLoans(),outFile, headers);
                }
                else if (it->first == "LoansHistory") // saving Loans history file
                {
                    headers = "ISBN,Book Name,Category,Loan Start Date,Loan End Date,Book Condition,Amount of Days the Book Was Loaned";
                    WriteToFile(it_c->second.GetLoanHistory(), outFile, headers);
                }
                else if (it->first == "Reservations") // saving resrtvations file
                {
                    headers = "ISBN,Book Name,Category,Reservation Date, Priority, Available Days";
                    WriteToFile(it_c->second.GetReservations(),outFile, headers);
                }
            }
            outFile.close();
        }
        catch (const exception& e) // in case there is an error
        {
            cerr << "Error opening file: " << it->second << " - " << e.what() << endl;
            return;
        }
    }
    if (type == "Outer") // save the max loan into file
        SaveMaxLoanToFile();
}

void DSM::LoanBook(const string& filter, const string& ID)
{// check if the customer can loan the book by its ISBN
    int copies = 0, matches = 0; string ISBN;
    int maxloans = m_Customers.find(ID)->second.GetMaxCustomerLoans();
    int currloans = m_Customers.find(ID)->second.GetCurrLoans();
    unordered_map<string, Book>::const_iterator itISBN = m_booksISBN.find(filter);
    unordered_multimap<string, const Book*>::const_iterator itName = m_booksName.find(filter);
    if (itISBN == m_booksISBN.end() && itName == m_booksName.end()) // check for the book existence in the library
    {
        cout << "Your desired book does not exist in the library!" << endl << endl;
        return;
    }
    if (itISBN != m_booksISBN.end()) // extract the ISBN
        ISBN = filter;
    else if (itName != m_booksName.end()) // extract the ISBN
        ISBN = BookNameCount(filter ,matches);
    copies = m_booksISBN.find(ISBN)->second.GetNloans();
    if (copies > 0 && !m_Customers.find(ID)->second.CheckLoanBook(ISBN)) { return; } // check if the customer can loan the book
    else if (copies <= 0 || currloans >= maxloans)  // check if copies exists or more than max loans
    {
        if (copies <= 0)
            cout << "No copies of this book are currently available." << endl << endl;
        else if (currloans >= maxloans)
            cout << "Maximum loan limit reached. Unable to borrow more books." << endl << endl;
        m_Customers.find(ID)->second.ReserveBookISBN(ISBN, m_booksISBN); // put the book in reserves
        if (m_ResQ.find(ISBN) == m_ResQ.end()) // ensure priority_queue exists for that ISBN
        {  
            // Initialize the priority queue with a ResCompare comparator using the given ISBN
            m_ResQ.emplace(ISBN, priority_queue<Customer*, vector<Customer*>, ResCompare>(ResCompare(ISBN)));
        }
        // Ensure the comparator is correctly used when accessing the priority queue
        m_ResQ.find(ISBN)->second.push(&m_Customers.find(ID)->second);
        cout << "Book successfully added to your reservations list!" << endl << endl;
        return;
    }
    m_Customers.find(ID)->second.PrintLoanBookNameISBN(ISBN, m_booksISBN); // loan the book by ISBN
    m_booksISBN.find(ISBN)->second.SetNloans(copies - 1); // update amount of copies in ISBN DB
}

string DSM::BookNameCount(const string& name,  int matches) const
{// checks if there are multiple books matching a name and returns the corresponding ISBN.
    string ISBN;
    matches = m_booksName.count(name);
    if (matches > 1)
        ISBN = BooksNamesMatches(name, matches);
    else
        ISBN = m_booksName.find(name)->second->GetID();
    return ISBN;
}

string DSM::BooksNamesMatches(const string& name, int matches) const
{ // prints book details matching the given name and prompts for a valid ISBN, ensuring the correct book is selected.
    string ISBN;
    cout << "There are " << matches << " matches for the book name : " << endl << endl;
    unordered_multimap<string, const Book*>::const_iterator it = m_booksName.find(name);
    while (it != m_booksName.end() && it->first == name)
    {
        it->second->PrintBook(); // print book details
        cout << "---------------------------------------------------" << endl;
        it++; // move to the next element
    }
    // the customer selects from the found results
    while (NOT_VALID_ISBN(ISBN) || m_booksISBN.find(ISBN) == m_booksISBN.end() || m_booksISBN.find(ISBN)->second.GetName() != name)
    {
        cout << "Please enter the ISBN of you desired book: "; getline(cin, ISBN);
        if (NOT_VALID_ISBN(ISBN) || m_booksISBN.find(ISBN) == m_booksISBN.end() || m_booksISBN.find(ISBN)->second.GetName() != name)
            cout << endl << "Invalid ISBN number" << endl << endl;
    }
    return ISBN;
}

void DSM::RemoveCustomerFromQueue(const string& ISBN, const string& ID)
{ /* removes a specific customer from the reservation queue for a given book ISBN 
     by rebuilding the queue without that customer. */
    unordered_map<string, priority_queue<Customer*, vector<Customer*>, ResCompare>>::iterator it;
    it = m_ResQ.find(ISBN); // check if the ISBN exists in the queue map

    priority_queue<Customer*, vector<Customer*>, ResCompare> tempQueue;
    tempQueue = priority_queue<Customer*, vector<Customer*>, ResCompare>(ResCompare(ISBN));
    priority_queue<Customer*, vector<Customer*>, ResCompare>& origQueue = it->second;
    while (!origQueue.empty()) // rebuild the queue without the customer to remove
    {
        Customer* current = origQueue.top();
        origQueue.pop();

        if (current != &m_Customers.find(ID)->second)
            tempQueue.push(current);
    }
    it->second.swap(tempQueue); // replace the original queue with the new one
}

void DSM::AddBook()
{// prompts for valid book details, checks for duplicates, and updates the system once valid data is entered.
    string ISBN, name, authors, publisher, category, copies, maxloan;
    while (NOT_VALID_ISBN(ISBN) ||
        m_booksISBN.find(ISBN) != m_booksISBN.end())
    {
        cout << "Please enter the ISBN of the book: "; getline(cin, ISBN); // enters ISBN
        if (NOT_VALID_ISBN(ISBN))
            cout << endl << "Invalid ISBN number" << endl << endl;
        else if (m_booksISBN.find(ISBN) != m_booksISBN.end())
            cout << "This ISBN already exists in the library." << endl << endl;
    }
    while (name == "") // enters name
    {
        cout << "Please enter the name of the book: "; getline(cin, name);
        if (name.empty())
            cout << "Books name cannot be empty." << endl << endl;
    }
    cout << "Please enter the authors of the book: "; getline(cin, authors); // enter authors
    while (NOT_VALID_CATEGORY(category)) // enters category
    {
        cout << "Please enter the category of the book: "; getline(cin, category);
        if (NOT_VALID_CATEGORY(category))
            cout << endl << "Invalid category" << endl << endl;
    }
    cout << "Please enter the publisher of the book: "; getline(cin, publisher); // enters publisher
    while (NOT_VALID_NUMBER(copies) || copies == "0") // enters copies of the book
    {
        cout << "Please enter the number of copies for the book: "; getline(cin, copies);
        if (NOT_VALID_NUMBER(copies) || copies == "0")
            cout << endl << "Your input is not a number or equals to 0." << endl << endl;
    }
    while (NOT_VALID_NUMBER(maxloan) || maxloan == "0") // enters max loan duration
    {
        cout << "Enter the max loan for the book (in days): "; getline(cin, maxloan);
        if (NOT_VALID_NUMBER(maxloan) || maxloan == "0")
            cout << endl << "Your input is not a number or equals to 0." << endl << endl;
    }
    UpdateAddingBookDS(ISBN, name, authors, publisher, category, stoi(copies), stoi(maxloan));
    cout << endl << "Book added successfully to the library!" << endl << endl;
}

void DSM::UpdateAddingBookDS(const string& ISBN, const string& name,
    const string& authors, const string& publisher, const string& category,
    int copies, int maxloan)
{ // puts the new book object in the ISBN DS and pointers to the other DS
    m_booksISBN.emplace(ISBN, Book(ISBN, name, authors, publisher, category, copies, maxloan));
    if (category.find('+') != string::npos) // contain multiple categories
        MultiCategoriesHandle(category, m_booksISBN.find(ISBN)->second);
    else
        m_booksCategory.emplace(category, &m_booksISBN.find(ISBN)->second);
    m_booksName.emplace(name, &m_booksISBN.find(ISBN)->second);
    m_booksAuthors.emplace(authors, &m_booksISBN.find(ISBN)->second);
    m_booksPublisher.emplace(publisher, &m_booksISBN.find(ISBN)->second);

    m_booksSimLoan.emplace(to_string(copies), &m_booksISBN.find(ISBN)->second);
    m_booksLoanTime.emplace(to_string(maxloan), &m_booksISBN.find(ISBN)->second);
}

void DSM::RemoveBook(const string& ISBN)
{ // validates the ISBN, checks if the book exists, and removes it if valid.
    if (NOT_VALID_ISBN(ISBN)) // check if valid ISBN
    {
        cout << "Invalid ISBN" << endl;
        return;
    }
    else if (!FindBookISBN<Book>(ISBN, m_booksISBN, false)) // search for such ISBN
    {
        cout << "No book found with ISBN : " << ISBN << endl;
        return;
    }
    else // ISBN has been found
        UpdateRemovingBooksDS(ISBN, "");
    cout << "The book was successfully removed!" << endl << endl;
}

void DSM::UpdateRemovingBooksDS(const string& ISBN, const string& category)
{ // removes a book from all relevant data structures before erasing it from m_booksISBN.
    unordered_map<string, Book>::iterator bookIt = m_booksISBN.find(ISBN);
    if (category.size() != 0) // handle if category exists
        UpdateCategory(bookIt, category); // update for new category
    if (bookIt->second.GetCategory().empty() || category.empty())
    {
        RemoveBookPtr(m_booksName, &bookIt->second); // remove in the name DS
        RemoveBookPtr(m_booksAuthors, &bookIt->second); // remove in the authors DS
        RemoveBookPtr(m_booksPublisher, &bookIt->second); // remove in the publisher DS
        RemoveBookPtr(m_booksCategory, &bookIt->second); // remove in the category DS
        RemoveBookPtr(m_booksSimLoan, &bookIt->second); // remove in the simultanious loan DS
        RemoveBookPtr(m_booksLoanTime, &bookIt->second); // remove in the loan time DS

        m_ResQ.erase(ISBN); // remove from ResQ
        RemoveBookReservationsnLoans(ISBN); // removes from reservations and loans
        m_booksISBN.erase(ISBN); // erase the book
    }
}

void DSM::RemoveBookReservationsnLoans(const string& ISBN)
{// removes a deleted book from the reservations and loans lists
    unordered_map<string, Customer>::iterator it;
    for (it = m_Customers.begin(); it != m_Customers.end(); it++)
    {
        it->second.RemoveFromRes(ISBN);
        it->second.DeleteLoan(ISBN);
    }
}

void DSM::RemoveBookPtr(unordered_multimap<string, const Book*>& booksMap, const Book* bookPtr)
{ // remove the book pointers
    unordered_multimap<string, const Book*>::iterator it;
    for (it = booksMap.begin(); it != booksMap.end(); )
    {
        if (it->second->GetID() == bookPtr->GetID())
            it = booksMap.erase(it); // erase returns the next valid iterator
        else
            it++; // only increment if not erasing
    }
}

void DSM::AddClient()
{// validates and collects a unique customer ID, name, and password, then adds the customer to the database.
    string ID, name, password;
    do
    { 
        cout << endl << "Enter the new customer ID: ";
        getline(cin, ID);
        if (ID.find_first_not_of("0123456789") != string::npos) // check for valid ID
            cout << "Invalid ID, please enter a unique ID." << endl;
        else if (m_Customers.find(ID) != m_Customers.end() ||
            m_librarians.find(ID) != m_librarians.end()) // check if the ID already exists in others DBs
            cout << "This ID is already taken, please enter a unique ID." << endl;
        else if (ID.size() < MIN_ID_LENGTH) // check if ID too short
            cout << "This ID is too short, please enter a another ID." << endl;
        else if (ID.size() > MAX_ID_LENGTH)
            cout << "This ID is too long, please enter a another ID." << endl;
    } while (m_Customers.find(ID) != m_Customers.end() ||
        m_librarians.find(ID) != m_librarians.end() || ID.size() < MIN_ID_LENGTH || ID.size() > MAX_ID_LENGTH ||
        ID.find_first_not_of("0123456789") != string::npos);  // repeat if the ID is not entered correctly

    cout << endl << "Enter the new customer name: ";
    getline(cin, name); // enters customers name

    cout << endl << "Enter the new customer password: ";
    getline(cin, password); // enters customers password

    m_Customers.emplace(ID, Customer(ID, name, password)); // add to the database
    cout << "Customer added successfully!" << endl << endl;
}

void DSM::AddCategory(const string& category)
{ // adds a category if it doesn't exist, else prints a message.
    if (m_booksCategory.find(category) != m_booksCategory.end()) // category already exists
    {
        cout << "Category " << "\"" << category << "\"" << " already exists" << endl << endl;
        return;
    }
    else if (NOT_VALID_CATEGORY(category)) // check valid category
    {
        cout << "Invalid Category" << endl << endl;
        return;
    }
    else
    {
        if (category.find('+') != string::npos) // handles multiple category 
        {
            cout << "Categories ";
            stack<string> categories = ParseCategories(category);
            while (!categories.empty())
            {
                m_booksCategory.emplace(categories.top(), nullptr); // add to books by category
                cout << "\"" << categories.top() << "\", ";
                categories.pop();
            }
        }
        else // handles one category
        {
            m_booksCategory.emplace(category, nullptr);
            cout << "Category " << "\"" << category << "\"";
        }
        cout << " was added successfully" << endl << endl;
    }
}

void DSM::RemoveClient(const string& ID)
{// removes a customer from the database if the ID exists; otherwise, it prints an error message.
    int copies = 0;
    if (m_Customers.find(ID) != m_Customers.end()) // check if customer is exists
    {
        array<stack<string>, 2> updates = m_Customers.find(ID)->second.DeleteClientData();
        while(!updates[0].empty()) // books that has been loaned 
        {
            copies = m_booksISBN.find(updates[0].top())->second.GetNloans();
            m_booksISBN.find(updates[0].top())->second.SetNloans(copies + 1);
            updates[0].pop();
        }
        while (!updates[1].empty()) // books that has been in reservations
        {
            RemoveCustomerFromQueue(updates[1].top(), ID);
            updates[1].pop();
        }    
        m_Customers.erase(ID);
        cout << "The customer with ID " << ID << " was successfully deleted from the library database.";
        cout << endl << endl;
    }
    else // ID not found
        cout << "No customer found with the provided ID." << endl << endl;
}

void DSM::RemoveCatogory(const string& category)
{ // removes a book category and all associated books from the library database if the category exists; 
    // otherwise, it prints an error message.
    if (m_booksCategory.count(category) == 0) // category did not found
    {
        cout << "The selected category was not found." << endl << endl;
        return;
    }

    stack<string> StoreISBN; // storing the ISBN numebers to delete
    unordered_multimap<string, const Book*>::const_iterator it = m_booksCategory.find(category);

    while (it != m_booksCategory.end() && it->first == category && it->second != nullptr)
    {
        StoreISBN.push(it->second->GetID()); // stores ISBN to remove from category
        it++;
    }

    m_booksCategory.erase(category); // delete category

    while (StoreISBN.size() > 0)
    {
        UpdateRemovingBooksDS(StoreISBN.top(), category); // updates other DSs
        StoreISBN.pop();
    }
    cout << "The category has been successfully removed." << endl << endl;
}

void DSM::CallUpdateBook(const string& ISBN)
{// updates specified book fields based on user input, ensuring valid data before applying the changes.

    if (m_booksISBN.find(ISBN) == m_booksISBN.end()) // check for existence of the book
    {
        cout << "No such book in the library" << endl << endl;
        return;
    }  
    string input, name, authors, publisher, category, sim, max;
    unsigned start = 0, i = 0, end;
    cout << "Which fields would you like to update? " << endl;
    cout << "Name, Authors, Publisher, Category, Simultanious loans or Maximum loan time? " << endl;
    cout << "Your choice is: ";
    getline(cin, input);
    const string fields[] = { "Name","Authors", "Publisher", "Category", "Simultanious loans","Maximum loan time" };
    string update[] = { name,authors, publisher, category, sim, max };

    // loop while there's another comma or unprocessed input to handle the last field. 
    while ((end = input.find(',', start)) != string::npos || start < input.size())
    {
        string field = input.substr(start, end - start);

        for (; i < FIELDS; i++)
        {
            if (field == fields[i])
            {
                cout << "Enter the " + fields[i] + " to update: ";
                getline(cin, update[i]);
                if (IS_NUMBER(i, update[i]) || update[i] == "0") // need to insert number to the field (max loan and time)
                {
                    do
                    {
                        if (update[i] == "0") { cout << "This parameter cannot be 0." << endl; }
                        else { cout << "Your input contains non-digit characters" << endl; }
                        cout << "Enter the " + fields[i] + " to update: ";
                        getline(cin, update[i]);
                    } while (IS_NUMBER(i, update[i]) || update[i] == "0"); // check input number
                }
                UpdatingBook(i, update[i], ISBN);
                i++;
                break;
            }
        }
        if (end == string::npos) // nothing left to process in the input string
            break;
        start = end + 2; // move past the comma
    }
    cout << endl <<"The book parameters have been successfully updated." << endl << endl;
}

void DSM::UpdatingBook(int setter, const string& change, const string& ISBN)
{// updates the specified book field in various data structures based on the provided setter, 
    // modifying the book's attributes in the DSM.
    Book& book = m_booksISBN.find(ISBN)->second;
    unordered_multimap<string, const Book*>::iterator it;
    switch (setter)
    {
    case 0: // updating name
    {
        UpdateBookField(m_booksName, &Book::GetName, &Book::SetName, change, book);
        break;
    }
    case 1: // updating author
    {
        UpdateBookField(m_booksAuthors, &Book::GetAuthors, &Book::SetAuthors, change, book);
        break;
    }
    case 2: // updating publisher
    {
        UpdateBookField(m_booksPublisher, &Book::GetPublisher, &Book::SetPublisher, change, book);
        break;
    }
    case 3: // updating category
    {
        LibUpdatesCategory(book,change);
        break;
    }
    case 4: // updating simultanious loans
    {

        UpdateBookField(m_booksSimLoan, &Book::GetNloans, &Book::SetNloans, change, book);
        break;
    }
    case 5:// updating loan time
    {
        UpdateBookField(m_booksLoanTime, &Book::GetMaxLoanTime, &Book::SetMaxLoanTime, change, book);
        break;
    }
    }
}

void DSM::UpdateLibrarian(const string& ID, int what2print)
{ // update librarian credentials
    string cred = ChangeCredentials<Librarian>(ID, m_librarians, what2print);
    UpdateUsersDB<Librarian>(cred, m_librarians);
}

void DSM::UpdateCustomer(const string& ID, int what2print)
{// update customer credentials
    string cred = ChangeCredentials(ID, m_Customers, what2print);
    UpdateUsersDB<Customer>(cred, m_Customers);
}

void DSM::AutomaticReturns()
{// function that check if have some book that need to return automatic according the end date loan
    Date today;
    unordered_map<string, Customer>::iterator it; // itertor for all the cunstomers
    for (it = m_Customers.begin(); it != m_Customers.end(); it++)
    {
        unordered_map<string, Loan>::const_iterator loanIt = it->second.GetLoanIteratorBegin();

        while (loanIt != it->second.GetLoanIteratorEnd())
        {
            const Loan& loan = loanIt->second;

            Date loanEndDate(loan.GetEndDate());

            if (loanEndDate <= today) // check if loan date has passed
            {
                m_PrintAutoReturn.emplace(it->second.GetID(), loan.GetID()); // add for next printing system msg
                unordered_map<string, Book>::iterator bookIt = m_booksISBN.find(loan.GetID());
                loanIt++;
                it->second.Move2History(bookIt->first, today); // move loan to history
                if (bookIt != m_booksISBN.end())
                {
                    int copies = bookIt->second.GetNloans();
                    bookIt->second.SetNloans(copies + 1);  // update numbers of copies
                }
            }
            else
                ++loanIt;
        }
    }
}

void DSM::PrintResReturnMSG(const string& ID)
{ // print the system meesage for automatic loan and reservations
    unordered_multimap<string, string>::iterator it_res = m_PrintLoansnISBN.find(ID);
    unordered_multimap<string, string>::iterator it_return = m_PrintAutoReturn.find(ID);
    // Check if there is a message to print
    if (it_res != m_PrintLoansnISBN.end() || it_return != m_PrintAutoReturn.end())
    {
        string header = "SYSTEM MESSAGE:";
        cout << endl << header << endl << string(header.size(), '=');

        if (it_res != m_PrintLoansnISBN.end()) // prints reservations msg
        {
            const string ResMsg = "The book with ISBN: " + it_res->second +
                " has been added to your loan list from reservations.";
            PrintsSystemMSG(it_res, m_PrintLoansnISBN, ID, ResMsg);
        }

        if (it_return != m_PrintAutoReturn.end()) // prints returns msg
        {
            const string AutoReturnMSG = "The book with ISBN: " + it_return->second +
                " has been returned to the library and added to your loan history.";
            PrintsSystemMSG(it_return, m_PrintAutoReturn, ID, AutoReturnMSG);
        }
        cout << endl;
    }
}

void DSM::PrintsSystemMSG(unordered_multimap<string, string>::iterator& it, unordered_multimap<string, string>& database,
    const string& ID, const string& msg)
{//prints the res or return msg to screen
    while (it != database.end() && it->first == ID) // ensure we only iterate for this key
    {
        cout << endl << msg << endl;
        it = database.erase(it);  // erase and move to the next valid iterator
    }
}

void DSM::AvailableFromRes(const string& ID)
{  // processes book reservations, assigning available copies to customers in the reservation queue 
   // while updating loan records and customer statuses.
    unordered_map<string, priority_queue<Customer*, vector<Customer*>, ResCompare>>::iterator it;
    Date StartDate;
    for (it = m_ResQ.begin(); it != m_ResQ.end(); it++)
    {
        // gets all parameters
        Date EndDate;
        EndDate.add(m_booksISBN.find(it->first)->second.GetMaxLoanTime());
        const string name = m_booksISBN.find(it->first)->second.GetName();
        const string category = m_booksISBN.find(it->first)->second.GetCategory();
        const string ISBN = m_booksISBN.find(it->first)->second.GetID();
        if (m_booksISBN.find(it->first)->second.GetNloans() > 0) // found a book avaliable to loan
        {
            priority_queue<Customer*, vector<Customer*>, ResCompare> pqCopy = it->second;
            vector<Customer*> customers;
            vector<Customer*>::iterator CusIt;
            CopyPQ(pqCopy, customers);
            for (CusIt = customers.begin(); CusIt != customers.end();) // iterates through all customers
            {
                int currloans = (*CusIt)->GetCurrLoans();
                if (currloans + 1 <= (*CusIt)->GetMaxCustomerLoans()) // current loans is + 1 is less than the maximum  
                {
                    int copies = m_booksISBN.find(ISBN)->second.GetNloans();
                    m_booksISBN.find(ISBN)->second.SetNloans(copies - 1);
                    (*CusIt)->LoanBookNameISBN(ISBN, m_booksISBN); // loans the book
                    (*CusIt)->RemoveFromRes(ISBN); // remove from reservations
                    m_PrintLoansnISBN.emplace((*CusIt)->GetID(), ISBN); // add for next printing system msg
                    RemoveCustomerFromQueue(ISBN, (*CusIt)->GetID());
                    CusIt = customers.erase(CusIt);
                    break;
                }
                else
                    CusIt++;
            }
        }
    }
}

void DSM::CopyPQ(priority_queue<Customer*, vector<Customer*>, ResCompare>& pq,
    vector<Customer*>& customers) const
{ // transfers all customers from a priority queue into a vector while preserving their order.
    while (!pq.empty())
    {
        customers.push_back(pq.top());
        pq.pop();
    }
}

stack<string> DSM::ParseCategories(const string& categories) const
{// parsing a category string by + sign, puts all the parts in stack
    stack<string> result;
    stringstream ss(categories);
    string category;

    while (getline(ss, category, '+'))
        result.push(category);

    return result;
}

void DSM::MultiCategoriesHandle(const string& categories, const Book& book)
{// handles a situation where multiple categories where inserted of imported from books file
    stack<string> ParsedCat = ParseCategories(categories);
    while (!ParsedCat.empty())
    {
        Book* pointer = new Book(book);
        m_MulCategories.push(pointer); // Store in stack for easy deleting
        m_booksCategory.emplace(ParsedCat.top(), pointer); // books by category
        ParsedCat.pop();
    }
}

void DSM::UpdateCategory(unordered_map<string, Book>::iterator book, const string& cat)
{// removes a categpry from DSs
    const string category = book->second.GetCategory();
    int index = category.find(cat);
    if (index == 0) // category is at the begining of the string
    {
        if (category[index+cat.size()] == '+') // deleting the specific category from the start
            book->second.SetCategory(category.substr(index+ cat.size() + 1)); 
        else
            book->second.SetCategory(""); // deleting the only catrgory the book has left
    }
    else // category is is not at first in the cat string
    {
        const string UpdatedCat = category.substr(0, index - 1) + category.substr(index + cat.size());
        book->second.SetCategory(UpdatedCat);
    }
}

void DSM::LibUpdatesCategory(Book& book, const string& change)
{// updating the catrgory by the librarian
    stack<string> CatBefore = ParseCategories(book.GetCategory());
    unordered_multimap<string, const Book*>::iterator it;

    while (!CatBefore.empty()) // loops over the categories in the stack
    {
        string category = CatBefore.top();
        CatBefore.pop();

        unordered_multimap<string, const Book*>::iterator rangeStart = m_booksCategory.find(category);
        unordered_multimap<string, const Book*>::iterator rangeEnd = m_booksCategory.end();

        for (it = rangeStart; it != rangeEnd; ++it) // iterate through all values that match the category
        {
            if (it->first != category) // moved past a different category situation
                break;

            if (it->second->GetID() == book.GetID()) // check ISBN match
            {
                m_booksCategory.erase(it);
                break;
            }
        }
    }
    book.SetCategory(change); // sets new category
    MultiCategoriesHandle(change, book);
}