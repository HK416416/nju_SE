#include "../include/Transaction.h"
#include <iostream>

Transaction::Transaction(const std::string& id, double amt, TransactionType t, 
                         const std::string& desc, const std::string& cat, const std::string& accId)
    : recordId(id), amount(amt), type(t), description(desc), category(cat), accountId(accId) {
    transactionTime = std::chrono::system_clock::now();
}

std::string Transaction::getRecordId() const { return recordId; }
double Transaction::getAmount() const { return amount; }
TransactionType Transaction::getType() const { return type; }
std::chrono::system_clock::time_point Transaction::getTransactionTime() const { return transactionTime; }
std::string Transaction::getDescription() const { return description; }
std::string Transaction::getCategory() const { return category; }
std::string Transaction::getAccountId() const { return accountId; }

void Transaction::display() const {
    std::cout << "记录ID: " << recordId 
              << " | 金额: " << amount 
              << " | 类型: " << (type == TransactionType::INCOME ? "收入" : "支出")
              << " | 分类: " << category 
              << " | 描述: " << description << std::endl;
}