# Library Management System

## Overview

This project is a **Library Management System** designed to handle digital book lending. The application provides functionalities for managing books, users (both librarians and customers), and reservations. The project was implemented using **C++** with an **Object-Oriented Programming (OOP)** approach.

## Features

- **User Authentication**: Login system for both librarians and customers.
- **Book Management**: Add, remove, and update books.
- **Customer Management**: Add, remove, and update customers.
- **Lending System**: Handle book loans, returns, and reservations.
- **Priority Queue for Reservations**: Books are assigned based on reservation priority.
- **Command Line Interface (CLI)**: Simple text-based interface for interaction.
- **Data Persistence**: All data is saved and retrieved from CSV files.

## Project Structure

The project is divided into two main parts:

1. **Static Library (********`OOPLib`********)**: Contains all the logic and data structures for the application.
2. **Executable (********`Main`********)**: Provides the CLI-based user interface.

## Classes and Responsibilities

### Core Classes

- **Library\_App**: Manages the main application workflow.
- **DSM (Data Structure Manager)**: Handles storage and management of books, customers, and librarians.
- **User**: Base class for `Customer` and `Librarian`.
- **Customer**: Manages book loans, history, and reservations.
- **Librarian**: Handles book and customer management.
- **Book**: Represents books with attributes such as ISBN, title, author, and category.
- **Loan**: Tracks book borrowings.
- **LoanHistory**: Maintains records of past loans.
- **Reservation**: Handles future book reservations.
- **Date**: Utility class for handling dates.

## Data Structures Used

- **Unordered Map**: Used for fast lookup of books, users, and reservations.
- **Unordered Multimap**: Supports multiple values per key (e.g., books by the same author).
- **Priority Queue**: Manages book reservations by priority and date.
- **Stack**: Used for dynamic memory management and cleanup.

## File Storage

Data is stored in CSV files:

- `Loaners.csv`: Contains customer details.
- `Librarians.csv`: Contains librarian details.
- `Books.csv`: Stores book information.
- `xxxxx_loans.csv`: Tracks ongoing book loans.
- `xxxxx_loanHistory.csv`: Stores completed book loans.
- `xxxxx_reservations.csv`: Manages future reservations.

## How to Run the Project

1. **Compile the project**:
   ```sh
   g++ -o library_app Main.cpp -L. -lOOPLib
   ```
2. **Run the application**:
   ```sh
   ./library_app
   ```

## Future Improvements

- Implement a graphical user interface (GUI).
- Improve error handling and input validation.
- Enhance search functionality with advanced filtering.

## Authors

- **Ido Ben Harosh** ([ido.ben.harosh@e.braude.ac.il](mailto\:ido.ben.harosh@e.braude.ac.il))
- **Yuval Hammer** ([Y.H@e.braude.ac.il](mailto\:Y.H@e.braude.ac.il))

## License

This project is licensed under the MIT License.
