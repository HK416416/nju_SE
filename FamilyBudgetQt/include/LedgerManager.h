#ifndef LEDGER_MANAGER_H
#define LEDGER_MANAGER_H

#include <vector>
#include <map>
#include <memory>
#include <string>
#include "Transaction.h"
#include "Budget.h"
#include "AlertService.h"

class LedgerManager {
private:
    std::vector<Transaction> transactions;
    std::map<std::string, Budget> budgets;
    std::unique_ptr<AlertService> alertService;
    double totalIncome;
    double totalExpense;

public:
    LedgerManager();
    
    // 交易记录管理
    void importTransaction(const Transaction& transaction);
    void importTransactions(const std::vector<Transaction>& newTransactions);
    
    // 预算管理
    void importBudget(const Budget& budget);
    void importBudgets(const std::vector<Budget>& newBudgets);
    
    // 预警相关功能
    void updateBudgetSpending(const std::string& category, double amount);
    
    // 显示功能
    void displayAllBudgets() const;
    void displayFinancialOverview() const;
    void displayAlertHistory() const;
    void displayTransactionsByCategory(const std::string& category) const;
    
    // ============ 新增方法 ============
    // 获取所有交易记录
    std::vector<Transaction> getTransactions() const;
    
    // 获取所有预算（返回副本）
    std::vector<Budget> getBudgets() const;
    
    // 获取所有预算（返回引用，可修改）
    std::vector<Budget>& getBudgetsRef();
    
    // 获取指定分类的预算
    Budget* getBudgetByCategory(const std::string& category);
    
    // 获取交易总数
    size_t getTransactionCount() const;
    
    // 获取预算总数
    size_t getBudgetCount() const;
    
    // 获取总收入
    double getTotalIncome() const;
    
    // 获取总支出
    double getTotalExpense() const;
    
    // 获取净收入
    double getNetIncome() const;
    
    // 获取所有预警
    std::vector<std::string> getAllAlerts() const;
    
    // 添加支出到指定预算分类
    bool addExpenseToBudget(const std::string& category, double amount);
    
    // 清空所有数据
    void clearAllData();
    
    // 获取按分类统计的支出
    std::map<std::string, double> getExpenseByCategory() const;
    
    // 获取按分类统计的收入
    std::map<std::string, double> getIncomeByCategory() const;
};

#endif // LEDGER_MANAGER_H