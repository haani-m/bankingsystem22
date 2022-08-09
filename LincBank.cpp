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
#include<cmath> //needed for implementing interest calc
#include <ctime> //needed for transaction timestamp
#include <stdlib.h> //needed for converting user input to double

using namespace std;

char pound = 156;


//------------transaction class--------


class Transaction {
	string desc;
	string timestamp;
	double value;
public:

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
		cout << "	." << getdesc() << " : " << pound << value << " on " << timestamp;
	};

	static void historytoString(vector <Transaction> h) {
		for (Transaction i : h)
			i.toString();
		cout << endl;
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


//0000000000000000000         interfaces       00000000000000000000
class InterestEarning {
// pure virtual to ensure that the method is implemented and overridden in savings class
public:
	virtual void computeInterest(int t) = 0;
};


//00000000000000000000		sub		classes		000000000000000000

//------	Current account		------

class Current : public Account {
	double overdraft = 500; //overdraft remaining
public:
	
	//constructor
	Current(string t, int c, double b, Transaction tr) {
		settype(t);
		setcode(c);
		setbalance(b);
		setovoverdraft(500);
		appendhistory(tr);
	}

	//getters and setters 
	double getoverdraft(void) { return overdraft; }
	void setovoverdraft(double o) { overdraft = o; }
	void addoverdraft(double o) { overdraft += o; }
	void minusoverdraft(double o) { overdraft -= o; }


	//methods
	void Account::deposit(double depam) {

		//aglorithms for processing deposit when in overdraft
		if (getbalance() < 0) {
			
			if (overdraft + depam >= 500) {
				setbalance(depam - (500 - overdraft));
			}

			else if (depam + overdraft < 500) {
				overdraft += depam;
				addbalance(depam);
			}
		}

		else {addbalance(depam);}

		Transaction dep("Deposit", depam);
		appendhistory(dep);
		toString();
	};

	
	void Account::withdraw(double witam) {
		
		//processing for withdrawls when in overdraft
		if (getbalance()<0){
			if (witam > overdraft) {
				cout << "Insufficient funds" << endl;}

			else {
				minusbalance(witam);
				minusoverdraft(witam);
				Transaction wit("Withdrawl", -witam);
				toString();
			}
		}

		else {
			if (witam > (getbalance() + overdraft))
				cout << "Insufficient Fund" << endl;

			else if (witam > getbalance() && witam < (getbalance() + overdraft)) {
				overdraft -= (witam);
				minusbalance(witam);
				Transaction wit("Withdrawl", -witam);
				toString();
			}

			else { 
				minusbalance(witam);
				Transaction wit("Withdrawl", -witam);
				toString();
			}
			
		}

	};

	void Account::toString() {
		cout << gettype();
		cout << " Account | Balance: " << pound << getbalance();
		cout << " | Overdraft Remaining: " << pound << getoverdraft() << endl;
		Transaction::historytoString(gethistory());
	};

};




//-----Savings account------
class Savings : public Account, public InterestEarning {
	double interestRate = 0.0085;
	const double isa = 0.0115;
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

	//secondary constructor for creating temp object,
	//as pointer can't use the calculate interesty since it is exclusive to the savings sub class
	Savings(double b, double ir) {
		setbalance(b);
		setinterestRate(ir);
	}

	//getters and setters
	double getinterestRate(void) { return interestRate; }
	void setinterestRate(double ir) { interestRate = ir; }


	//methods

	void InterestEarning::computeInterest(int t) {
		int n = 12;
		double P = getbalance();
		double r = getinterestRate();
		double projected = P * (pow((1 + (r / n)), (n * t)));
		cout << "Projected balance in " << t << " years: "<< pound << projected << endl;
	};


	void Account::deposit(double depam) {
		
		addbalance(depam);
		Transaction dep("Deposit", depam);
		appendhistory(dep);
		toString();

	};

	void Account::withdraw(double witam) {
		
		if (witam > getbalance())
			cout << "Insufficient funds" << endl;

		else {
			minusbalance(witam);
			Transaction dep("Withdrawl", -witam);
			appendhistory(dep);
			toString();
		}

	};

	void Account::toString() {
		cout << gettype();
		cout << " Account | Balance " << pound << getbalance();
		cout << " | Interest Rate: " << 100 * interestRate << "%" << endl;
		Transaction::historytoString(gethistory());
	};

};


	
int main()
{	
	//-------------------testing-----------------------
	int test = 0;
	if (test == 1) {
		//testing transaction + current constructor and pointer
		Transaction trtest("transaction pointer test", 1000.47);

		Current crtest("pointer Current", 1, 1000.47, trtest);

		//trtest.toString();
		//crtest.toString();

		Account* testptr = &crtest;

		testptr->toString();
		cout << endl;
		cout << "overdaft test: " << testptr->getoverdraft() << endl;

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
	} //--------------------- end of testing-------------------




	std::vector <std::string> parameters;
	std::string userCommand;
	
	//http://www2.lawrence.edu/fast/GREGGJ/CMSC270/Pointers/objects_and_pointers.html#:~:text=A%20pointer%20is%20a%20type,new%20object%20of%20type%20Order.

	//vector of open account pointers
	vector <Account*> openacc = {};
	
	//account pointer:
	//polymorphism allows super class pointer to be used for sub class objects properly
	Account* accountptr = nullptr; 
	
	int isaCount = 0;
	
	

	std::cout << "~~~ Welcome to LincBank! ~~~" << std::endl;
	
	//following options copied from assignment example execution document
	cout << "open type initial_deposit: open a current (1), savings (2) or ISA (3) account" << endl;
	cout << "view [index]: view balance and recent transactions" << endl;
	cout << "withdraw sum: withdraw funds from most recently viewed account" << endl;
	cout << "deposit sum: deposit funds into most recently viewed account" << endl;
	cout << "transfer src dest sum: transfer funds between accounts" << endl;
	cout << "project years: project balance forward in time" << endl;
	cout << "exit: close this application" << endl;
	cout << "options: view these options again" << endl;

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
			cout << "open type initial_deposit: open a current (1), savings (2) or ISA (3) account" << endl;
			cout << "view [index]: view balance and recent transactions" << endl;
			cout << "withdraw sum: withdraw funds from most recently viewed account" << endl;
			cout << "deposit sum: deposit funds into most recently viewed account" << endl;
			cout << "transfer src dest sum: transfer funds between accounts" << endl;
			cout << "project years: project balance forward in time" << endl;
			cout << "exit: close this application" << endl;
			cout << "options: view these options again" << endl;
			
		}

		//open
		else if (command.compare("open") == 0)
		{

			try {


				//error handling for inputs that do not satisfy the number of parameters required
				if (size(parameters) > 3 || size(parameters)<3)
					throw "Number of parameters invalid.";


				int input2 = stoi(parameters[1]);

				if (input2 > 3 || input2 < 1)
					throw "Invalid account type";

				double input3 = stod(parameters[2]);

				if (input3 < 0)
					throw "Deposit amount cannot be negative.";

				//--------------OPEN CURRENT
				if (input2 == 1) {

					Transaction temp("Opening Deposit", input3);
					accountptr = new Current("Current", 1, input3, temp);

					openacc.push_back(accountptr);
					cout << "Current Account created:" << endl;
					accountptr->toString();

				}

				//--------------OPEN SAVINGS
				else if (input2 == 2) {

					Transaction temp("Opening Deposit", input3);
					accountptr = new Savings("Savings", 2, input3, temp);

					openacc.push_back(accountptr);
					cout << "Savings Account created:" << endl;
					accountptr->toString();

				}

				//-------------------OPEN ISA
				else if (input2 == 3) {
					if (isaCount == 1) {
						throw "You can only open 1 ISA account.";
					}
					else {
						if (input3 >= 1000) {

							Transaction temp("Opening Deposit", input3);
							accountptr = new Savings("ISA", 3, input3, temp);

							openacc.push_back(accountptr);
							cout << "ISA Account created:" << endl;
							accountptr->toString();
							temp.toString();
							isaCount = 1;
						}

						else {
							throw "Minimum deposit of 1000.00 required for ISA accounts";
						}
					}
				}
			}

			catch (const char* msg) {
				cerr << "Error has occured:" << endl << msg << endl;
			}

			catch (...) {
				cout << "Default Error has occurred:" << endl << "Ensure you input a valid parameters etc." << endl;
			}

		}	


		//-------------- VIEW
		else if (command.compare("view") == 0)
		{

			try {
				
				// present in all commands that deal with open accounts
				if (size(openacc) == 0)
					throw "Please open an account before you proceed.";


				//checks no. of params
				if (size(parameters) > 2)
					throw "Invalid number of parameters.";

				//NO INDEX
				if (size(parameters) == 1) {
					for (Account* i : openacc) {
						i->toString();
					}
				}

				//INDEX

				else if (size(parameters) == 2) {
					
					int index = stoi(parameters[1]) - 1;

					//checks if index is in range
					if (index > size(openacc)-1)
						throw "Invalid index.";

					accountptr = openacc[index];
					accountptr->toString();
				}

			}

			catch (const char* msg) {
				cerr <<"Error has occured:"<< endl << msg << endl;
			}
			catch (...) {
				cerr << "Default Error has occurred:"<< endl << "Ensure you input valid parameters etc." << endl;
			}

		}

		//------------- WITHDRAW
		else if (command.compare("withdraw") == 0)
		{
			try {

				if (size(openacc) == 0)
					throw "Please open an account before you proceed.";

				//checks for no. of params
				if (size(parameters) > 2 || size(parameters) < 2)
					throw "Invalid number of parameters.";

				//checks for negative
				if (stod(parameters[1]) <= 0)
					throw "Withdrawl amount cannot be negative.";

				accountptr->withdraw(stod(parameters[1]));
			}

			catch (const char* msg) {
				cerr << "Error has occured:" << endl << msg << endl;
			}
			catch (...) {
				cerr << "Defualt Error has occurred:" << endl << "Ensure you input valid parameters etc." << endl;
			}

		}

		//--------------- DEPOSIT
		else if (command.compare("deposit") == 0)
		{	
			try {

				if (size(openacc) == 0)
					throw "Please open an account before you proceed.";

				//checks no. of params
				if (size(parameters) > 2 || size(parameters) < 2)
					throw "Invalid number of parameters";

				//checks for negative
				if (stod(parameters[1]) <= 0)
					throw "Deposit amount cannot be negative.";

				accountptr->deposit(stod(parameters[1]));
			}

			catch (const char* msg) {
				cerr << "Error has occured:" << endl << msg << endl;
			}
			catch (...) {
				cerr << "Default Error has occurred:" << endl << "Ensure you input valid parameters etc." << endl;
			}

		}

		//--------------- TRANSFER
		else if (command.compare("transfer") == 0)
		{	
			
			
			try {

				if (size(openacc) == 0)
					throw "Please open an account before you proceed.";

				//checks number of params
				if (size(parameters) > 3 || size(parameters) < 3)
					throw "Invalid number of parameters.";
				

				int send = stoi(parameters[1]) - 1;
				int receive = stoi(parameters[2]) - 1;
				double transAm = stod(parameters[3]);

				//checks indexes are in range of vector
				if (send + 1 > size(openacc) || receive + 1 > size(openacc))
					throw "Invalid index.";
					
				accountptr = openacc[send];

				//if sending from current account, required for overdraft implementation
				if (accountptr->getcode() == 1) {

					if (transAm > (accountptr->getbalance() + accountptr->getoverdraft()))
						throw "Insufficient funds.";

					else {
						accountptr->withdraw(transAm);
						accountptr = openacc[receive];
						accountptr->deposit(transAm);
					}
				}

				//for transferring from savings account, overdraft is not needed to implement
				else {
					if (transAm > accountptr->getbalance()) {
						throw "Insufficient funds.";
					}

					else {
						accountptr->withdraw(transAm);
						accountptr = openacc[receive];
						accountptr->deposit(transAm);
					}
				}
			}
			
			catch (const char* msg) {
				cerr << "Error has occured:" << endl << msg << endl;
			}
			catch (...) {
				cerr << "Default Error has occurred:" << endl << "Ensure you input valid parameters etc." << endl;
			}
		}

		//------------- PROJECT 
		else if (command.compare("project") == 0){

			try {

				if (size(openacc) == 0)
					throw "Please open an account before you proceed.";

				if (size(parameters) > 2 || size(parameters) < 2)
					throw "Invalid number of parameters.";

				int y = stoi(parameters[1]);

				if (accountptr->getcode() == 1)
					throw  "Current accounts do not accrue interest, please select a Savings or ISA account (view to select).";

				Savings temp(accountptr->getbalance(), accountptr->getinterestRate());
				temp.computeInterest(y);
				
			}
		
			catch (const char* msg) {
				cerr << "Error has occured:" << endl << msg << endl;
			}
			catch (...) {
				cerr << "Default Error has occurred:" << endl << "Ensure you input valid parameters etc." << endl;
			}
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