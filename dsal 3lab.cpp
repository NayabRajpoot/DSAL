#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cmath>
using namespace std;

// ACCOUNT
class Account {
protected:
    int accountId;
    string name;
    double balance;
    unsigned int permissions;
    vector<double> transactions;

public:
    Account(int id, string n, double bal, unsigned int perm)
        : accountId(id), name(n), balance(bal), permissions(perm) {
    }

    virtual void deposit(double amount) = 0;
    virtual void withdraw(double amount) = 0;
    virtual void saveToFile() = 0;
    virtual void loadFromFile() = 0;

    virtual void display() {
        cout << "Account ID: " << accountId << "\nName: " << name << "\nBalance: " << balance << endl;
    }

    virtual ~Account() {}

    // accountId
    int getAccountId() const { return accountId; }
};

// Savings 
class SavingsAccount : public Account {
public:
    SavingsAccount(int id, string n, double bal, unsigned int perm)
        : Account(id, n, bal, perm) {
    }

    void deposit(double amount) override {
        if (permissions & 2) {
            balance += amount;
            transactions.push_back(amount);
            cout << "Deposited: " << amount << endl;
        }
        else {
            cout << "Permission denied: Cannot deposit." << endl;
        }
    }

    void withdraw(double amount) override {
        if (permissions & 1) {
            if (balance >= amount) {
                balance -= amount;
                transactions.push_back(-amount);
                cout << "Withdrawn: " << amount << endl;
            }
            else {
                cout << "Insufficient balance." << endl;
            }
        }
        else {
            cout << "Permission denied: Cannot withdraw." << endl;
        }
    }

    void saveToFile() override {
        ofstream file("account_" + to_string(accountId) + ".txt");
        file << "ACCOUNT Savings\n";
        file << accountId << " " << name << " " << balance << " " << permissions << endl;
        file << "TRANSACTIONS\n";
        for (double t : transactions) {
            file << t << endl;
        }
        file.close();
    }

    void loadFromFile() override {
        ifstream file("account_" + to_string(accountId) + ".txt");
        string line;
        getline(file, line); 
        file >> accountId >> name >> balance >> permissions;
        getline(file, line); 
        double t;
        while (file >> t) {
            transactions.push_back(t);
        }
        file.close();
    }
};

// CURRENT ACC
class CurrentAccount : public Account {
    double overdraftLimit;

public:
    CurrentAccount(int id, string n, double bal, unsigned int perm, double limit = 1000)
        : Account(id, n, bal, perm), overdraftLimit(limit) {
    }

    void withdraw(double amount) override {
        if (permissions & 1) {
            if (balance + overdraftLimit >= amount) {
                balance -= amount;
                transactions.push_back(-amount);
                cout << "Withdrawn: " << amount << endl;
            }
            else {
                cout << "Insufficient balance (including overdraft)." << endl;
            }
        }
        else {
            cout << "Permission denied: Cannot withdraw." << endl;
        }
    }

    void deposit(double amount) override {
        if (permissions & 2) {
            balance += amount;
            transactions.push_back(amount);
            cout << "Deposited: " << amount << endl;
        }
        else {
            cout << "Permission denied: Cannot deposit." << endl;
        }
    }

    void saveToFile() override {
        ofstream file("account_" + to_string(accountId) + ".txt");
        file << "ACCOUNT Current\n";
        file << accountId << " " << name << " " << balance << " " << permissions << " " << overdraftLimit << endl;
        file << "TRANSACTIONS\n";
        for (double t : transactions) {
            file << t << endl;
        }
        file.close();
    }

    void loadFromFile() override {
        ifstream file("account_" + to_string(accountId) + ".txt");
        string line;
        getline(file, line); // Skip ACC Current
        file >> accountId >> name >> balance >> permissions >> overdraftLimit;
        getline(file, line); // Skip TRANS
        double t;
        while (file >> t) {
            transactions.push_back(t);
        }
        file.close();
    }
};

// PERMISSIONS
unsigned int setPermissions(bool canWithdraw, bool canDeposit, bool canTransfer, bool isVIP) {
    unsigned int perm = 0;
    if (canWithdraw) perm |= 1;
    if (canDeposit) perm |= 2;
    if (canTransfer) perm |= 4;
    if (isVIP) perm |= 8;
    return perm;
}

// TRANSA
unsigned int encodeTransaction(int type, double amount) {
    unsigned int encoded = (type & 0xF) << 28;
    encoded |= (static_cast<unsigned int>(abs(amount)) & 0xFFFFFFF);
    return encoded;
}

void decodeTransaction(unsigned int encoded, int& type, double& amount) {
    type = (encoded >> 28) & 0xF;
    amount = static_cast<double>(encoded & 0xFFFFFFF);
    if (type == 2 || type == 3) amount *= -1; // WITHDRAW OR TRANS
}

// File Encryption
void encryptFile(const string& filename, char key) {
    ifstream in(filename);
    ofstream out("encrypted_" + filename);
    char c;
    while (in.get(c)) {
        out << (c ^ key);
    }
    in.close();
    out.close();
}

// Main Function
int main() {
    vector<Account*> accounts;
    int choice, id, type, withdrawPerm, depositPerm, transferPerm, vipPerm;
    string name;
    double balance, amount, limit;
    unsigned int permissions;

    do {
        cout << "\n===== Banking System Menu =====\n";
        cout << "1. Create Account\n2. Deposit\n3. Withdraw\n4. Show Account\n5. Save to File\n6. Load from File\n7. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1: { // Create Account
            cout << "\n--- Create New Account ---\n";

            
            cout << "Enter account type (1 for Savings, 2 for Current): ";
            cin >> type;
            while (type != 1 && type != 2) {
                cout << "Invalid type. Enter 1 for Savings or 2 for Current: ";
                cin >> type;
            }

            
            cout << "Enter account ID: ";
            cin >> id;

          
            cout << "Enter account holder name: ";
            cin.ignore(); 
            getline(cin, name);

        
            cout << "Enter initial balance: ";
            cin >> balance;
            while (balance < 0) {
                cout << "Balance cannot be negative. Enter again: ";
                cin >> balance;
            }

            //  permissions
            cout << "Set permissions (1 for Yes, 0 for No):\n";
            cout << "Can withdraw? (1/0): ";
            cin >> withdrawPerm;
            cout << "Can deposit? (1/0): ";
            cin >> depositPerm;
            cout << "Can transfer? (1/0): ";
            cin >> transferPerm;
            cout << "Is VIP? (1/0): ";
            cin >> vipPerm;
            permissions = setPermissions(withdrawPerm, depositPerm, transferPerm, vipPerm);

            
            if (type == 1) {
                accounts.push_back(new SavingsAccount(id, name, balance, permissions));
            }
            else {
                cout << "Enter overdraft limit: ";
                cin >> limit;
                accounts.push_back(new CurrentAccount(id, name, balance, permissions, limit));
            }
            cout << "Account created successfully!\n";
            break;
        }

        case 2: { // Deposit
            cout << "\n--- Deposit ---\n";
            cout << "Enter account ID: ";
            cin >> id;
            cout << "Enter amount to deposit: ";
            cin >> amount;
            for (Account* acc : accounts) {
                if (acc->getAccountId() == id) {
                    acc->deposit(amount);
                    break;
                }
            }
            break;
        }

        case 3: { // Withdraw
            cout << "\n--- Withdraw ---\n";
            cout << "Enter account ID: ";
            cin >> id;
            cout << "Enter amount to withdraw: ";
            cin >> amount;
            for (Account* acc : accounts) {
                if (acc->getAccountId() == id) {
                    acc->withdraw(amount);
                    break;
                }
            }
            break;
        }

        case 4: { // Show Account
            cout << "\n--- Account Details ---\n";
            cout << "Enter account ID: ";
            cin >> id;
            for (Account* acc : accounts) {
                if (acc->getAccountId() == id) {
                    acc->display();
                    break;
                }
            }
            break;
        }

        case 5: { // Save to File
            cout << "\n--- Save Accounts to File ---\n";
            for (Account* acc : accounts) {
                acc->saveToFile();
            }
            cout << "All accounts saved to files.\n";
            break;
        }

        case 6: { // Load from File
            cout << "\n--- Load Accounts from File ---\n";
            for (Account* acc : accounts) {
                acc->loadFromFile();
            }
            cout << "All accounts loaded from files.\n";
            break;
        }

        case 7: // Exit
            cout << "Exiting...\n";
            break;

        default:
            cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 7);

    // CleaR
    for (Account* acc : accounts) {
        delete acc;
    }
    accounts.clear();

    return 0;
}
