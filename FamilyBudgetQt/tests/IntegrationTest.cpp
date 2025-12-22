#include <gtest/gtest.h>
#include <iostream>
#include "Transaction.h"
#include "Budget.h"
#include "LedgerManager.h"
#include "SampleDataGenerator.h"

// 集成测试：交易和预算的完整流程
TEST(IntegrationTest, CompleteBudgetTransactionFlow) {
    LedgerManager manager;
    
    // 创建预算
    Budget foodBudget("B001", "食品", 1000.0, BudgetPeriod::MONTHLY, 0.8);
    manager.importBudget(foodBudget);
    
    // 添加交易
    Transaction grocery("T001", 200.0, TransactionType::EXPENSE, "超市购物", "食品", "ACC001");
    manager.importTransaction(grocery);
    
    // 验证预算状态
    auto budgets = manager.getBudgets();
    ASSERT_FALSE(budgets.empty());
    EXPECT_DOUBLE_EQ(budgets[0].getCurrentSpent(), 200.0);
    EXPECT_DOUBLE_EQ(budgets[0].getUsageRate(), 0.2);
}

// 集成测试：批量导入示例数据
TEST(IntegrationTest, SampleDataImport) {
    LedgerManager manager;
    
    // 导入示例预算
    auto sampleBudgets = SampleDataGenerator::generateSampleBudgets();
    manager.importBudgets(sampleBudgets);
    
    // 导入示例交易
    auto sampleTransactions = SampleDataGenerator::generateSampleTransactions();
    manager.importTransactions(sampleTransactions);
    
    // 验证数据导入
    auto budgets = manager.getBudgets();
    auto transactions = manager.getTransactions();
    
    EXPECT_GT(budgets.size(), 0);
    EXPECT_GT(transactions.size(), 0);
}

// 集成测试：预警系统
TEST(IntegrationTest, AlertSystem) {
    LedgerManager manager;
    
    // 创建低预算并添加高支出
    Budget smallBudget("B002", "娱乐", 100.0, BudgetPeriod::MONTHLY, 0.5);
    manager.importBudget(smallBudget);
    
    // 添加超过预算50%的支出（应触发预警）
    Transaction expense("T002", 60.0, TransactionType::EXPENSE, "电影票", "娱乐", "ACC001");
    manager.importTransaction(expense);
    
    // 验证预算状态
    auto budgets = manager.getBudgets();
    ASSERT_FALSE(budgets.empty());
    EXPECT_DOUBLE_EQ(budgets[0].getUsageRate(), 0.6);
    EXPECT_GE(budgets[0].checkAlert(), AlertLevel::MILD);
}

// 集成测试：财务概览
TEST(IntegrationTest, FinancialOverview) {
    LedgerManager manager;
    
    // 添加收入和支出
    Transaction income("T003", 5000.0, TransactionType::INCOME, "工资", "工资", "ACC001");
    Transaction expense1("T004", 1000.0, TransactionType::EXPENSE, "房租", "住房", "ACC001");
    Transaction expense2("T005", 500.0, TransactionType::EXPENSE, "餐饮", "食品", "ACC001");
    
    manager.importTransaction(income);
    manager.importTransaction(expense1);
    manager.importTransaction(expense2);
    
    // 验证财务数据
    EXPECT_DOUBLE_EQ(manager.getTotalIncome(), 5000.0);
    EXPECT_DOUBLE_EQ(manager.getTotalExpense(), 1500.0);
    EXPECT_DOUBLE_EQ(manager.getNetIncome(), 3500.0);
    EXPECT_EQ(manager.getTransactionCount(), 3);
}

// int main(int argc, char **argv) {
//     ::testing::InitGoogleTest(&argc, argv);
    
//     std::cout << "==========================================" << std::endl;
//     std::cout << "  家庭记账本系统 - 集成测试" << std::endl;
//     std::cout << "==========================================" << std::endl;
    
//     return RUN_ALL_TESTS();
// }