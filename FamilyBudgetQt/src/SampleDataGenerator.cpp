#include "../include/SampleDataGenerator.h"
#include <iostream>
#include <codecvt>

std::vector<Transaction> SampleDataGenerator::generateSampleTransactions() {
    // 设置控制台输出为UTF-8以支持中文

    
    return {
        Transaction("T001", 150.0, TransactionType::EXPENSE, "Lunch", "Food", "ACC001"),
        Transaction("T002", 80.0, TransactionType::EXPENSE, "Transportation Card", "Transport", "ACC001"),
        Transaction("T003", 300.0, TransactionType::EXPENSE, "Supermarket", "Shopping", "ACC001"),
        Transaction("T004", 5000.0, TransactionType::INCOME, "Salary", "Income", "ACC001"),
        Transaction("T005", 200.0, TransactionType::EXPENSE, "Movie Ticket", "Entertainment", "ACC001"),
        Transaction("T006", 400.0, TransactionType::EXPENSE, "Clothes", "Shopping", "ACC001")
    };
}

std::vector<Budget> SampleDataGenerator::generateSampleBudgets() {
    return {
        Budget("B001", "Food", 500.0, BudgetPeriod::MONTHLY, 0.8),
        Budget("B002", "Transport", 300.0, BudgetPeriod::MONTHLY, 0.8),
        Budget("B003", "Shopping", 600.0, BudgetPeriod::MONTHLY, 0.75),
        Budget("B004", "Entertainment", 200.0, BudgetPeriod::MONTHLY, 0.9)
    };
}