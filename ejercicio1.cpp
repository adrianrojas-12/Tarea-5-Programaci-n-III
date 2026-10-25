#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <string>
using namespace std;

class Account {
public:
    Account(string id, double balance) : id_(id), balance_(balance) {}

    string getId() const { return id_; }
    double getBalance() const { return balance_; }

    double balance_;
private:
    string id_;
};

class Transaction {
public:
    Transaction(string type, string accountId, double amount) : type_(type), accountId_(accountId), amount_(amount) {}

    void print() const {
        cout << type_ << " " << amount_ << " en " << accountId_ << endl;
    }

private:
    string type_;
    string accountId_;
    double amount_;
};

class TransactionManager {
public:
    static TransactionManager& getInstance() {
        static TransactionManager instance;
        return instance;
    }

    TransactionManager(const TransactionManager&) = delete;
    TransactionManager& operator=(const TransactionManager&) = delete;

    void deposit(Account* account, double amount) {
        lock_guard<mutex> lock(mutex_);
        account->balance_ += amount;
        log_.push_back(Transaction("DEPOSIT", account->getId(), amount));
    }

    void withdraw(Account* account, double amount) {
        lock_guard<std::mutex> lock(mutex_);
        if (account->balance_ >= amount) {
            account->balance_ -= amount;
            log_.push_back(Transaction("WITHDRAW", account->getId(), amount));
        }
    }

    int transactionCount() const { return log_.size(); }

private:
    TransactionManager() {}

    mutex mutex_;
    vector<Transaction> log_;
};

int main() {
    auto& manager = TransactionManager::getInstance();

    // Caso 1: main del enunciado (10 depósitos de 100 y 10 retiros de 50)
    Account account("A1", 1000);
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&]() {
            manager.deposit(&account, 100);
            manager.withdraw(&account, 50);
        });
    }
    for (auto& t : threads) t.join();
    std::cout << "A1 balance final: " << account.getBalance() << std::endl;

    // Caso 2: solo retiros, cumple 1000 - (10 * 50)
    Account account2("A2", 1000);
    std::vector<std::thread> threads2;
    for (int i = 0; i < 10; ++i) {
        threads2.emplace_back([&]() {
            manager.withdraw(&account2, 50);
        });
    }
    for (auto& t : threads2) t.join();
    std::cout << "A2 balance final: " << account2.getBalance() << std::endl;
    std::cout << "Direccion manager: " << &TransactionManager::getInstance() << std::endl;
    std::cout << "Transacciones registradas: " << manager.transactionCount() << std::endl;
}