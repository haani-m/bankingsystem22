/* ------------------------------------------------------
CMP2801M: Advanced Programming
Driver program for "LincBank" Assessment Item 1
Semester A 2021

Written by James Brown (jamesbrown@lincoln.ac.uk)

During marking, we will use the exact same notation
as provided in the brief, so make sure you follow that guideline.

You should avoid removing parts of this driver program, but
add to it the necessary code to produce your implementation.

Good luck!

Haani Mahmood
ID: 20782374
------------------------------------------------------ */
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include<cmath>
#include <ctime>

using namespace std;

char pound = 156;
//------------transaction class--------
class Transaction {
	string desc;
	string timestamp;
	double value;
public:

	Transaction(string d, string ts, double v) {
		desc = d;
		timestamp = ts;
		value = v;
	};

	Transaction(string d, double v) {
		desc = d;

		//date and time snippet sourced from:
		//https://www.tutorialspoint.com/cplusplus/cpp_date_time.htm#:~:text=There%20are%20four%20time%2Drelated,as%20some%20sort%20of%20integer.&text=Following%20are%20the%20important%20functions,in%20C%20or%20C%2B%2B
		time_t now = time(0);
		char* dt = ctime(&now);
		timestamp = dt;

		value = v;
	}

	void toString() {
		cout << getdesc() << endl << pound << value << endl << timestamp << endl;
	};

	void historytoString(vector <Transaction> h) {

		for (Transaction i : h)
			i.toString();
	}

	void setdesc(string d) { desc = d; }
	string getdesc(void) { return desc; }

	void settimestamp(string t) { timestamp = t; }
	string gettimestamp(void) { return timestamp; }

	void setvalue(double v) { value = v; }
	double getvalue(void) { return value; }
};







//-------        Abstract Super class	  ---------
class Account {
private:				//private rather than protected 

	string type;		//contains name of type of account

	int code;			//contains corresponding code for type of account:  1 = current, 2 = savings, 3 = ISA	
						//makes handling different account types easier
	double balance = 0;
	vector <Transaction> history = {};

public:
	//pure virtual methods, required to achieve polymorphism
	//
	
	virtual void deposit(double depam) = 0;
	virtual void withdraw(double witam) = 0;
	virtual void toString() = 0;

	//virtual methods
	//some sub classes don't need some of these functions, therefore 
	// chose to make them virtual rather than pure virtual
	virtual double getoverdraft(void) { return 0; };
	virtual void setovoverdraft(double o) {};
	virtual void addoverdraft(double o) {};
	virtual void minusoverdraft(double o) {};

	//!!!!!!!!!!!!!!!!!!!!!!!
	virtual double getinterestRate(void) { return 0; };
	virtual void setinterestRate(double ir) {};

	//getters and setters, as well as other attribute modifiers
	void settype(string a) { type = a; }
	string gettype(void) { return type; }

	void setcode(int c) { code = c; }
	int getcode(void) { return code; }

	void setbalance(double b) { balance = b; }
	double getbalance(void) { return balance; }
	void addbalance(double b) { balance += b; }
	void minusbalance(double b) { balance -= b; }

	void appendhistory(Transaction c) { history.push_back(c); }
	vector <Transaction> gethistory() { return history; }

};


//interfaces
class InterestEarning {
// pure virtual to ensure that the method is implemented and overridden in savings class
public:
	virtual void computeInterest(int t) = 0;
};


//00000000000000000000		sub		classes		000000000000000000

//------	Current account		------

class Current : public Account {
	double overdraft = 500;
public:
	
	//constructor
	Current(string t, int c, double b, Transaction tr) {
		settype(t);
		setcode(c);
		setbalance(b);
		setovoverdraft(500);
		appendhistory(tr);
	}

	//getters and setters !!!!!!!!!!!!!!!!!!!!!!!!!
	double getoverdraft(void) { return overdraft; }
	void setovoverdraft(double o) { overdraft = o; }
	//void Account::addoverdraft(double o) { overdraft += o; }
	//void Account::minusoverdraft(double o) { overdraft -= o; }


	//methods
	void Account::deposit(double depam) {
		addbalance(depam);
	};

	void Account::withdraw(double witam) {
		minusbalance(witam);
	};

	void Account::toString() {
		cout << gettype();
		cout << " Account -- Balance " << pound << getbalance();
		cout << " -- Overdraft " << pound << getoverdraft() << endl;
		vector <Transaction> r = gethistory();
		
	};

};




//-----Savings account------
class Savings : public Account, public InterestEarning {
	double interestRate = 0.0085;
	double isa = 0.015;
public:
	//constructor

	Savings(string t, int c, double b, Transaction tr) {
		settype(t);
		setcode(c);
		setbalance(b);

		// if ISA account, changes interest rate to the isa rate
		// simplifies other methods, such as computeInterest method
		if (c == 3) {
			interestRate = isa;
		}
		appendhistory(tr);
	}

	//getters and setters
	double getinterestRate(void) { return interestRate; }
	void setinterestRate(double ir) { interestRate = ir; }

	//!?!?!?!?!?!?!may not need
	//double getisa(void) { return isa; }
	//void setisa(double i) { isa = i; }



	//methods

	void InterestEarning::computeInterest(int t) {
		double P = getbalance();
		int n = 12;
		float projected = P * (pow(1 + (interestRate / n), n * t));
		cout << "Projected balance in" << t << "years: £" << projected << endl;
	};


	void Account::deposit(double depam) {
		addbalance(depam);
	};

	void Account::withdraw(double witam) {
		minusbalance(witam);
	};

	void Account::toString() {
		cout << gettype();
		cout << " Account -- Balance £" << getbalance() << endl;
		cout << "Interest Rate: " << 100 * interestRate << endl;
	};



};


	
int main()
{	//-------------------testing-----------------------

	//testing transaction + current constructor and pointer
	Transaction trtest("transaction pointer test", 1000.47);

	Current crtest("pointer Current", 1, 1000.47, trtest );

	//trtest.toString();
	//crtest.toString();
	
	Account* testptr = &crtest;

	testptr->toString();
	cout << endl;
	cout << "overdaft test: " << testptr->getoverdraft() << endl ;

	vector <Transaction> histest = testptr->gethistory();

	for (Transaction i : histest)
		i.toString();

	//testing pointer and savings constructor

	cout << endl << endl << endl << "Savings test:" << endl;

	Transaction trtest2("Savings transaction test", 250.12);

	Savings svtest("Savings pointer test", 3, 250.12, trtest2);

	testptr = &svtest;

	vector <Transaction> histest2 = testptr->gethistory();

	for (Transaction i : histest2)
		i.toString();


	//-----------------testing-------------------------





	std::vector <std::string> parameters;
	std::string userCommand;
	// you may also want to store a collection of opened accounts here
	

	//http://www2.lawrence.edu/fast/GREGGJ/CMSC270/Pointers/objects_and_pointers.html#:~:text=A%20pointer%20is%20a%20type,new%20object%20of%20type%20Order.

	//vector of open account pointers
	vector <Account*> openacc = {};
	
	//account pointer
	Account* accountptr = nullptr; 
	//polymorphism allows pointer super class pointer to be used for sub class objects
	
	

	std::cout << "~~~ Welcome to LincBank! ~~~" << std::endl;

	while (userCommand != "exit")
	{
		parameters.clear(); // clear ready for next command
		std::cout << std::endl << ">>> ";

		std::getline(std::cin, userCommand);
		char* cstr = new char[userCommand.length() + 1];
		strcpy(cstr, userCommand.c_str());

		char* token;
		token = strtok(cstr, " ");

		while (token != NULL)
		{
			parameters.push_back(token);
			token = strtok(NULL, " ");
		}

		// Define all commands as per the brief
		std::string command = parameters[0];

		if (command.compare("options") == 0)
		{
			// display the various commands to the user
		}
		else if (command.compare("open") == 0)
		{
			
			// allow a user to open an account
			// e.g., Account* a = new Savings(...);
		}
		else if (command.compare("view") == 0)
		{
			// display an account according to an index (starting from 1)
			// alternatively, display all accounts if no index is provided
		}
		else if (command.compare("withdraw") == 0)
		{
			// allow user to withdraw funds from an account
		}
		else if (command.compare("deposit") == 0)
		{
			// allow user to deposit funds into an account
		}
		else if (command.compare("transfer") == 0)
		{
			// allow user to transfer funds between accounts
			// i.e., a withdrawal followed by a deposit!
		}
		else if (command.compare("project") == 0)
		{
			// compute compound interest t years into the future
		}
		//else if (command.compare("search"))
		//{
		//	allow users to search their account history for a transaction
		//  (this is a stretch task)
		//}

	}

	std::cout << "Press any key to quit...";
	std::getchar();
	
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file