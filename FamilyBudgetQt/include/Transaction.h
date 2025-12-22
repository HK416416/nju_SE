#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <chrono>
#include "CommonEnums.h"

class Transaction {
private:
    std::string recordId;
    double amount;
    TransactionType type;
    std::chrono::system_clock::time_point transactionTime;
    std::string description;
    std::string category;
    std::string accountId;

public:
    Transaction(const std::string& id, double amt, TransactionType t, 
                const std::string& desc, const std::string& cat, const std::string& accId);

    // Getter 方法
    std::string getRecordId() const;
    double getAmount() const;
    TransactionType getType() const;
    std::chrono::system_clock::time_point getTransactionTime() const;
    std::string getDescription() const;
    std::string getCategory() const;
    std::string getAccountId() const;

    void display() const;
};

#endif // TRANSACTION_H