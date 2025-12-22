#include "../include/LedgerManager.h"
#include <iostream>
#include <algorithm>

LedgerManager::LedgerManager() : totalIncome(0), totalExpense(0) {
    alertService = std::make_unique<AlertService>();
}

// ============ 交易记录管理 ============
void LedgerManager::importTransaction(const Transaction& transaction) {
    transactions.push_back(transaction);
    
    if (transaction.getType() == TransactionType::INCOME) {
        totalIncome += transaction.getAmount();
    } else {
        totalExpense += transaction.getAmount();
        updateBudgetSpending(transaction.getCategory(), transaction.getAmount());
    }
    
    std::cout << "成功导入消费记录: ";
    transaction.display();
}

void LedgerManager::importTransactions(const std::vector<Transaction>& newTransactions) {
    std::cout << "开始批量导入 " << newTransactions.size() << " 条消费记录..." << std::endl;
    
    for (const auto& transaction : newTransactions) {
        importTransaction(transaction);
    }
    
    std::cout << "批量导入完成！" << std::endl;
}

// ============ 预算管理 ============
void LedgerManager::importBudget(const Budget& budget) {
    budgets[budget.getCategory()] = budget;
    std::cout << "成功导入预算: ";
    budget.display();
}

void LedgerManager::importBudgets(const std::vector<Budget>& newBudgets) {
    std::cout << "开始批量导入 " << newBudgets.size() << " 个预算..." << std::endl;
    
    for (const auto& budget : newBudgets) {
        importBudget(budget);
    }
    
    std::cout << "批量导入完成！" << std::endl;
}

void LedgerManager::updateBudgetSpending(const std::string& category, double amount) {
    auto it = budgets.find(category);
    if (it != budgets.end()) {
        it->second.addExpense(amount);
        alertService->checkBudgetAlert(it->second);
    }
}

// ============ 新增方法实现 ============
std::vector<Transaction> LedgerManager::getTransactions() const {
    return transactions;
}

std::vector<Budget> LedgerManager::getBudgets() const {
    std::vector<Budget> budgetList;
    for (const auto& pair : budgets) {
        budgetList.push_back(pair.second);
    }
    return budgetList;
}

std::vector<Budget>& LedgerManager::getBudgetsRef() {
    static std::vector<Budget> budgetList;
    budgetList.clear();
    for (auto& pair : budgets) {
        budgetList.push_back(pair.second);
    }
    return budgetList;
}

Budget* LedgerManager::getBudgetByCategory(const std::string& category) {
    auto it = budgets.find(category);
    if (it != budgets.end()) {
        return &(it->second);
    }
    return nullptr;
}

size_t LedgerManager::getTransactionCount() const {
    return transactions.size();
}

size_t LedgerManager::getBudgetCount() const {
    return budgets.size();
}

double LedgerManager::getTotalIncome() const {
    return totalIncome;
}

double LedgerManager::getTotalExpense() const {
    return totalExpense;
}

double LedgerManager::getNetIncome() const {
    return totalIncome - totalExpense;
}

std::vector<std::string> LedgerManager::getAllAlerts() const {
    std::vector<std::string> alerts;
    
    for (const auto& pair : budgets) {
        AlertLevel level = pair.second.checkAlert();
        if (level != AlertLevel::NONE) {
            std::string alertMsg;
            if (level == AlertLevel::SEVERE) {
                alertMsg = "[严重] " + pair.second.getCategory() + "预算已超支！";
            } else {
                alertMsg = "[警告] " + pair.second.getCategory() + "预算使用率过高！";
            }
            alerts.push_back(alertMsg);
        }
    }
    
    return alerts;
}

bool LedgerManager::addExpenseToBudget(const std::string& category, double amount) {
    auto it = budgets.find(category);
    if (it != budgets.end()) {
        it->second.addExpense(amount);
        totalExpense += amount;  // 更新总支出
        alertService->checkBudgetAlert(it->second);
        return true;
    }
    return false;
}

void LedgerManager::clearAllData() {
    transactions.clear();
    budgets.clear();
    totalIncome = 0;
    totalExpense = 0;
    alertService = std::make_unique<AlertService>();  // 重新创建预警服务
}

std::map<std::string, double> LedgerManager::getExpenseByCategory() const {
    std::map<std::string, double> expenseMap;
    
    for (const auto& transaction : transactions) {
        if (transaction.getType() == TransactionType::EXPENSE) {
            expenseMap[transaction.getCategory()] += transaction.getAmount();
        }
    }
    
    return expenseMap;
}

std::map<std::string, double> LedgerManager::getIncomeByCategory() const {
    std::map<std::string, double> incomeMap;
    
    for (const auto& transaction : transactions) {
        if (transaction.getType() == TransactionType::INCOME) {
            incomeMap[transaction.getCategory()] += transaction.getAmount();
        }
    }
    
    return incomeMap;
}

// ============ 显示功能 ============
void LedgerManager::displayAllBudgets() const {
    std::cout << "\n=== 所有预算状态 ===" << std::endl;
    
    if (budgets.empty()) {
        std::cout << "暂无预算数据" << std::endl;
        return;
    }
    
    for (const auto& pair : budgets) {
        pair.second.display();
        
        AlertLevel level = pair.second.checkAlert();
        if (level != AlertLevel::NONE) {
            std::cout << "  ⚠️ 预警状态: " 
                      << (level == AlertLevel::MILD ? "轻度预警" : "重度预警") << std::endl;
        }
        std::cout << std::endl;
    }
}

void LedgerManager::displayFinancialOverview() const {
    std::cout << "\n=== 财务概览 ===" << std::endl;
    std::cout << "总收入: ¥" << totalIncome << std::endl;
    std::cout << "总支出: ¥" << totalExpense << std::endl;
    std::cout << "净收入: ¥" << (totalIncome - totalExpense) << std::endl;
    std::cout << "交易记录总数: " << transactions.size() << std::endl;
    std::cout << "预算总数: " << budgets.size() << std::endl;
}

void LedgerManager::displayAlertHistory() const {
    alertService->displayAlertHistory();
}

void LedgerManager::displayTransactionsByCategory(const std::string& category) const {
    std::cout << "\n=== 分类 [" << category << "] 的交易记录 ===" << std::endl;
    
    bool found = false;
    for (const auto& transaction : transactions) {
        if (transaction.getCategory() == category) {
            transaction.display();
            found = true;
        }
    }
    
    if (!found) {
        std::cout << "该分类下暂无交易记录" << std::endl;
    }
}