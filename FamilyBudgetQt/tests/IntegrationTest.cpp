#include <gtest/gtest.h>
#include <memory>
#include <thread>
#include <chrono>
#include "Transaction.h"
#include "Budget.h"
#include "LedgerManager.h"
#include "AlertService.h"
#include "SampleDataGenerator.h"
#include "CommonEnums.h"

// ============ 集成测试类 ============
class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 在每个测试前创建新的管理器
        ledgerManager = std::make_unique<LedgerManager>();
    }
    
    void TearDown() override {
        ledgerManager.reset();
    }
    
    std::unique_ptr<LedgerManager> ledgerManager;
};

// ============ 测试用例 ============

// 测试1: 交易记录与预算的集成
TEST_F(IntegrationTest, TransactionBudgetIntegration) {
    // 1. 创建预算
    Budget foodBudget("B001", "食品", 1000.0, BudgetPeriod::MONTHLY, 0.8);
    ledgerManager->importBudget(foodBudget);
    
    // 2. 创建交易记录（支出）
    Transaction grocery("T001", 300.0, TransactionType::EXPENSE, 
                       "超市购物", "食品", "ACC001");
    ledgerManager->importTransaction(grocery);
    
    // 3. 验证预算已更新
    auto budgets = ledgerManager->getBudgets();
    ASSERT_FALSE(budgets.empty());
    EXPECT_NEAR(budgets[0].getCurrentSpent(), 300.0, 0.01);
    EXPECT_NEAR(budgets[0].getUsageRate(), 0.3, 0.01);
    
    // 4. 添加更多支出，触发预警
    Transaction restaurant("T002", 500.0, TransactionType::EXPENSE,
                          "餐厅聚餐", "食品", "ACC001");
    ledgerManager->importTransaction(restaurant);
    
    // 5. 验证预警状态
    budgets = ledgerManager->getBudgets();
    EXPECT_NEAR(budgets[0].getCurrentSpent(), 800.0, 0.01);
    EXPECT_NEAR(budgets[0].getUsageRate(), 0.8, 0.01);
    
    AlertLevel alertLevel = budgets[0].checkAlert();
    EXPECT_EQ(alertLevel, AlertLevel::MILD);  // 达到80%阈值
    
    // 6. 添加超出预算的支出
    Transaction moreFood("T003", 300.0, TransactionType::EXPENSE,
                        "额外购物", "食品", "ACC001");
    ledgerManager->importTransaction(moreFood);
    
    // 7. 验证超支
    budgets = ledgerManager->getBudgets();
    EXPECT_NEAR(budgets[0].getCurrentSpent(), 1100.0, 0.01);
    EXPECT_NEAR(budgets[0].getUsageRate(), 1.1, 0.01);
    EXPECT_TRUE(budgets[0].getRemainingBudget() < 0);
    
    alertLevel = budgets[0].checkAlert();
    EXPECT_EQ(alertLevel, AlertLevel::SEVERE);  // 超支，严重预警
}

// 测试2: 多个预算分类的集成
TEST_F(IntegrationTest, MultipleBudgetCategories) {
    // 创建多个预算
    std::vector<Budget> budgets = {
        Budget("B001", "食品", 1000.0, BudgetPeriod::MONTHLY, 0.8),
        Budget("B002", "交通", 500.0, BudgetPeriod::MONTHLY, 0.9),
        Budget("B003", "娱乐", 300.0, BudgetPeriod::MONTHLY, 0.7)
    };
    
    ledgerManager->importBudgets(budgets);
    
    // 创建不同分类的交易
    std::vector<Transaction> transactions = {
        Transaction("T001", 200.0, TransactionType::EXPENSE, "买菜", "食品", "ACC001"),
        Transaction("T002", 100.0, TransactionType::EXPENSE, "加油", "交通", "ACC001"),
        Transaction("T003", 150.0, TransactionType::EXPENSE, "电影", "娱乐", "ACC001"),
        Transaction("T004", 400.0, TransactionType::INCOME, "工资", "收入", "ACC001"),
        Transaction("T005", 600.0, TransactionType::EXPENSE, "聚餐", "食品", "ACC001")
    };
    
    for (const auto& trans : transactions) {
        ledgerManager->importTransaction(trans);
    }
    
    // 验证财务统计
    EXPECT_NEAR(ledgerManager->getTotalIncome(), 400.0, 0.01);
    EXPECT_NEAR(ledgerManager->getTotalExpense(), 1050.0, 0.01);
    EXPECT_NEAR(ledgerManager->getNetIncome(), -650.0, 0.01);  // 净支出
    
    // 验证各分类支出统计
    auto expenseByCategory = ledgerManager->getExpenseByCategory();
    EXPECT_NEAR(expenseByCategory["食品"], 800.0, 0.01);
    EXPECT_NEAR(expenseByCategory["交通"], 100.0, 0.01);
    EXPECT_NEAR(expenseByCategory["娱乐"], 150.0, 0.01);
    
    // 验证各预算状态
    auto updatedBudgets = ledgerManager->getBudgets();
    ASSERT_EQ(updatedBudgets.size(), 3);
    
    // 食品预算使用率80%，应触发轻度预警
    EXPECT_NEAR(updatedBudgets[0].getUsageRate(), 0.8, 0.01);
    EXPECT_EQ(updatedBudgets[0].checkAlert(), AlertLevel::MILD);
    
    // 交通预算使用率20%，无预警
    EXPECT_NEAR(updatedBudgets[1].getUsageRate(), 0.2, 0.01);
    EXPECT_EQ(updatedBudgets[1].checkAlert(), AlertLevel::NONE);
    
    // 娱乐预算使用率50%，无预警（阈值70%）
    EXPECT_NEAR(updatedBudgets[2].getUsageRate(), 0.5, 0.01);
    EXPECT_EQ(updatedBudgets[2].checkAlert(), AlertLevel::NONE);

    // // 交通预算使用率20%，无预警
    // EXPECT_NEAR(updatedBudgets[0].getUsageRate(), 0.2, 0.01);
    // EXPECT_EQ(updatedBudgets[0].checkAlert(), AlertLevel::NONE);           
    // // 娱乐预算使用率50%，无预警（阈值70%）    
    // EXPECT_NEAR(updatedBudgets[1].getUsageRate(), 0.5, 0.01);     
    // EXPECT_EQ(updatedBudgets[1].checkAlert(), AlertLevel::NONE);          
    // // 食品预算使用率80%，应触发轻度预警     
    // EXPECT_NEAR(updatedBudgets[2].getUsageRate(), 0.8, 0.01);     
    // EXPECT_EQ(updatedBudgets[2].checkAlert(), AlertLevel::MILD);
}

// 测试3: 完整记账业务流程
TEST_F(IntegrationTest, CompleteAccountingWorkflow) {
    // 步骤1: 设置月度预算
    std::vector<Budget> monthlyBudgets = {
        Budget("B-M-01", "房租", 2000.0, BudgetPeriod::MONTHLY, 0.95),
        Budget("B-M-02", "水电煤", 500.0, BudgetPeriod::MONTHLY, 0.9),
        Budget("B-M-03", "食品杂货", 1500.0, BudgetPeriod::MONTHLY, 0.85),
        Budget("B-M-04", "交通", 800.0, BudgetPeriod::MONTHLY, 0.9),
        Budget("B-M-05", "娱乐", 1000.0, BudgetPeriod::MONTHLY, 0.8)
    };
    
    ledgerManager->importBudgets(monthlyBudgets);
    
    // 步骤2: 记录收入
    Transaction salary("T-INC-01", 8000.0, TransactionType::INCOME, 
                      "月工资", "工资收入", "ACC-SALARY");
    ledgerManager->importTransaction(salary);
    
    // 步骤3: 记录各项支出
    std::vector<Transaction> expenses = {
        Transaction("T-EXP-01", 2000.0, TransactionType::EXPENSE, "房租支付", "房租", "ACC-CASH"),
        Transaction("T-EXP-02", 350.0, TransactionType::EXPENSE, "电费", "水电煤", "ACC-CASH"),
        Transaction("T-EXP-03", 1200.0, TransactionType::EXPENSE, "超市采购", "食品杂货", "ACC-CARD"),
        Transaction("T-EXP-04", 150.0, TransactionType::EXPENSE, "水费", "水电煤", "ACC-CASH"),
        Transaction("T-EXP-05", 600.0, TransactionType::EXPENSE, "加油", "交通", "ACC-CARD"),
        Transaction("T-EXP-06", 400.0, TransactionType::EXPENSE, "聚餐", "娱乐", "ACC-CARD"),
        Transaction("T-EXP-07", 800.0, TransactionType::EXPENSE, "电子产品", "其他", "ACC-CARD"),
        Transaction("T-EXP-08", 300.0, TransactionType::EXPENSE, "买菜", "食品杂货", "ACC-CASH")
    };
    
    for (const auto& expense : expenses) {
        ledgerManager->importTransaction(expense);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));  // 模拟时间间隔
    }
    
    // 步骤4: 验证财务状况
    EXPECT_NEAR(ledgerManager->getTotalIncome(), 8000.0, 0.01);
    EXPECT_NEAR(ledgerManager->getTotalExpense(), 5800.0, 0.01);
    EXPECT_NEAR(ledgerManager->getNetIncome(), 2200.0, 0.01);
    EXPECT_EQ(ledgerManager->getTransactionCount(), 9);  // 1收入 + 8支出
    
    // 步骤5: 验证预算使用情况
    auto budgets = ledgerManager->getBudgets();
    
    // 房租预算: 2000/2000 = 100% (超支)
    auto rentBudget = std::find_if(budgets.begin(), budgets.end(),
        [](const Budget& b) { return b.getCategory() == "房租"; });
    ASSERT_NE(rentBudget, budgets.end());
    EXPECT_NEAR(rentBudget->getUsageRate(), 1.0, 0.01);
    EXPECT_EQ(rentBudget->checkAlert(), AlertLevel::SEVERE);
    
    // 水电煤预算: 500/500 = 100% (超支)
    auto utilitiesBudget = std::find_if(budgets.begin(), budgets.end(),
        [](const Budget& b) { return b.getCategory() == "水电煤"; });
    ASSERT_NE(utilitiesBudget, budgets.end());
    EXPECT_NEAR(utilitiesBudget->getUsageRate(), 1.0, 0.01);
    EXPECT_EQ(utilitiesBudget->checkAlert(), AlertLevel::SEVERE);
    
    // 食品杂货预算: 1500/1500 = 100% (刚好用完)
    auto groceryBudget = std::find_if(budgets.begin(), budgets.end(),
        [](const Budget& b) { return b.getCategory() == "食品杂货"; });
    ASSERT_NE(groceryBudget, budgets.end());
    EXPECT_NEAR(groceryBudget->getUsageRate(), 1.0, 0.01);
    EXPECT_EQ(groceryBudget->checkAlert(), AlertLevel::SEVERE);
    
    // 交通预算: 600/800 = 75% (正常)
    auto transportBudget = std::find_if(budgets.begin(), budgets.end(),
        [](const Budget& b) { return b.getCategory() == "交通"; });
    ASSERT_NE(transportBudget, budgets.end());
    EXPECT_NEAR(transportBudget->getUsageRate(), 0.75, 0.01);
    EXPECT_EQ(transportBudget->checkAlert(), AlertLevel::NONE);
    
    // 娱乐预算: 400/1000 = 40% (正常)
    auto entertainmentBudget = std::find_if(budgets.begin(), budgets.end(),
        [](const Budget& b) { return b.getCategory() == "娱乐"; });
    ASSERT_NE(entertainmentBudget, budgets.end());
    EXPECT_NEAR(entertainmentBudget->getUsageRate(), 0.4, 0.01);
    EXPECT_EQ(entertainmentBudget->checkAlert(), AlertLevel::NONE);
    
    // 步骤6: 验证预警数量
    auto alerts = ledgerManager->getAllAlerts();
    EXPECT_GE(alerts.size(), 3);  // 至少3个超支预警
    
    // 步骤7: 验证分类统计
    auto expenseStats = ledgerManager->getExpenseByCategory();
    EXPECT_NEAR(expenseStats["房租"], 2000.0, 0.01);
    EXPECT_NEAR(expenseStats["水电煤"], 500.0, 0.01);
    EXPECT_NEAR(expenseStats["食品杂货"], 1500.0, 0.01);
    EXPECT_NEAR(expenseStats["交通"], 600.0, 0.01);
    EXPECT_NEAR(expenseStats["娱乐"], 400.0, 0.01);
    EXPECT_NEAR(expenseStats["其他"], 800.0, 0.01);  // 没有预算的类别
}

// 测试4: 数据导入导出集成（模拟）
TEST_F(IntegrationTest, DataImportExportIntegration) {
    // 使用SampleDataGenerator生成测试数据
    auto sampleBudgets = SampleDataGenerator::generateSampleBudgets();
    auto sampleTransactions = SampleDataGenerator::generateSampleTransactions();
    
    // 导入数据
    ledgerManager->importBudgets(sampleBudgets);
    ledgerManager->importTransactions(sampleTransactions);
    
    // 验证数据导入正确
    EXPECT_EQ(ledgerManager->getBudgetCount(), sampleBudgets.size());
    EXPECT_EQ(ledgerManager->getTransactionCount(), sampleTransactions.size());
    
    // 计算预期总收入/支出
    double expectedIncome = 0.0;
    double expectedExpense = 0.0;
    
    for (const auto& trans : sampleTransactions) {
        if (trans.getType() == TransactionType::INCOME) {
            expectedIncome += trans.getAmount();
        } else {
            expectedExpense += trans.getAmount();
        }
    }
    
    // 验证财务数据
    EXPECT_NEAR(ledgerManager->getTotalIncome(), expectedIncome, 0.01);
    EXPECT_NEAR(ledgerManager->getTotalExpense(), expectedExpense, 0.01);
    
    // 验证预算更新
    auto budgets = ledgerManager->getBudgets();
    for (const auto& budget : budgets) {
        // 每个预算应该都有对应的支出
        double categoryExpense = 0.0;
        for (const auto& trans : sampleTransactions) {
            if (trans.getCategory() == budget.getCategory() && 
                trans.getType() == TransactionType::EXPENSE) {
                categoryExpense += trans.getAmount();
            }
        }
        
        EXPECT_NEAR(budget.getCurrentSpent(), categoryExpense, 0.01);
    }
}

// 测试5: 边界条件集成测试
TEST_F(IntegrationTest, BoundaryConditions) {
    // 测试1: 零预算
    Budget zeroBudget("B-ZERO", "测试", 0.0, BudgetPeriod::MONTHLY, 0.5);
    ledgerManager->importBudget(zeroBudget);
    
    Transaction expense1("T001", 100.0, TransactionType::EXPENSE, "测试支出", "测试", "ACC001");
    ledgerManager->importTransaction(expense1);
    
    auto budgets = ledgerManager->getBudgets();
    ASSERT_FALSE(budgets.empty());
    EXPECT_NEAR(budgets[0].getCurrentSpent(), 100.0, 0.01);
    EXPECT_NEAR(budgets[0].getUsageRate(), 0.0, 0.01);  // 零预算时使用率为0
    EXPECT_EQ(budgets[0].checkAlert(), AlertLevel::SEVERE);  // 任何支出都会触发超支预警
    
    // 测试2: 极小金额交易
    Budget smallBudget("B-SMALL", "小额", 10.0, BudgetPeriod::MONTHLY, 0.9);
    ledgerManager->importBudget(smallBudget);
    
    Transaction tinyExpense("T002", 0.01, TransactionType::EXPENSE, "极小支出", "小额", "ACC001");
    for (int i = 0; i < 100; i++) {
        ledgerManager->importTransaction(tinyExpense);
    }
    
    // 测试3: 极大金额
    Budget largeBudget("B-LARGE", "大额", 1000000.0, BudgetPeriod::MONTHLY, 0.8);
    ledgerManager->importBudget(largeBudget);
    
    Transaction largeExpense("T003", 999999.99, TransactionType::EXPENSE, "大额支出", "大额", "ACC001");
    ledgerManager->importTransaction(largeExpense);
    
    // 测试4: 混合类型交易
    Transaction largeIncome("T004", 500000.0, TransactionType::INCOME, "大额收入", "投资", "ACC001");
    ledgerManager->importTransaction(largeIncome);
    
    // 验证混合后的财务状态
    EXPECT_NEAR(ledgerManager->getTotalIncome(), 500000.0, 0.01);
    EXPECT_NEAR(ledgerManager->getTotalExpense(), 100.01 + 999999.99, 0.01);  // 小额+大额
}

// 测试6: 并发操作集成测试（简化版本）
TEST_F(IntegrationTest, ConcurrencyIntegration) {
    // 创建预算
    Budget concurrentBudget("B-CONC", "并发测试", 10000.0, BudgetPeriod::MONTHLY, 0.9);
    ledgerManager->importBudget(concurrentBudget);
    
    // 模拟并发添加交易（顺序执行模拟）
    const int NUM_TRANSACTIONS = 100;
    double totalExpense = 0.0;
    
    for (int i = 0; i < NUM_TRANSACTIONS; i++) {
        double amount = 10.0 + (i % 20);  // 10-29之间的金额
        Transaction trans("T-CONC-" + std::to_string(i), amount, 
                         TransactionType::EXPENSE, "并发交易" + std::to_string(i),
                         "并发测试", "ACC001");
        ledgerManager->importTransaction(trans);
        totalExpense += amount;
    }
    
    // 验证总支出正确
    EXPECT_NEAR(ledgerManager->getTotalExpense(), totalExpense, 0.01);
    
    // 验证预算支出正确
    auto budgets = ledgerManager->getBudgets();
    ASSERT_FALSE(budgets.empty());
    EXPECT_NEAR(budgets[0].getCurrentSpent(), totalExpense, 0.01);
    
    // 计算预期使用率
    double expectedUsage = totalExpense / 10000.0;
    EXPECT_NEAR(budgets[0].getUsageRate(), expectedUsage, 0.01);
}

// 测试7: 错误恢复集成测试
TEST_F(IntegrationTest, ErrorRecoveryIntegration) {
    // 测试数据一致性
    Budget budget1("B1", "分类1", 1000.0, BudgetPeriod::MONTHLY);
    Budget budget2("B2", "分类2", 2000.0, BudgetPeriod::MONTHLY);
    
    ledgerManager->importBudget(budget1);
    ledgerManager->importBudget(budget2);
    
    // 添加一些交易
    ledgerManager->importTransaction(
        Transaction("T1", 500.0, TransactionType::EXPENSE, "支出1", "分类1", "ACC001"));
    ledgerManager->importTransaction(
        Transaction("T2", 1000.0, TransactionType::EXPENSE, "支出2", "分类2", "ACC001"));
    ledgerManager->importTransaction(
        Transaction("T3", 2000.0, TransactionType::INCOME, "收入", "工资", "ACC001"));
    
    // 记录当前状态
    double initialTotalIncome = ledgerManager->getTotalIncome();
    double initialTotalExpense = ledgerManager->getTotalExpense();
    size_t initialTransactionCount = ledgerManager->getTransactionCount();
    
    // 模拟"清除所有数据"操作
    ledgerManager->clearAllData();
    
    // 验证数据已清空
    EXPECT_EQ(ledgerManager->getTotalIncome(), 0.0);
    EXPECT_EQ(ledgerManager->getTotalExpense(), 0.0);
    EXPECT_EQ(ledgerManager->getTransactionCount(), 0);
    EXPECT_EQ(ledgerManager->getBudgetCount(), 0);
    
    // 重新导入相同数据
    ledgerManager->importBudget(budget1);
    ledgerManager->importBudget(budget2);
    ledgerManager->importTransaction(
        Transaction("T1", 500.0, TransactionType::EXPENSE, "支出1", "分类1", "ACC001"));
    ledgerManager->importTransaction(
        Transaction("T2", 1000.0, TransactionType::EXPENSE, "支出2", "分类2", "ACC001"));
    ledgerManager->importTransaction(
        Transaction("T3", 2000.0, TransactionType::INCOME, "收入", "工资", "ACC001"));
    
    // 验证恢复后的状态与之前相同
    EXPECT_NEAR(ledgerManager->getTotalIncome(), initialTotalIncome, 0.01);
    EXPECT_NEAR(ledgerManager->getTotalExpense(), initialTotalExpense, 0.01);
    EXPECT_EQ(ledgerManager->getTransactionCount(), initialTransactionCount);
}

// 测试8: 性能集成测试
TEST_F(IntegrationTest, PerformanceIntegration) {
    // 测试大量数据处理能力
    const int NUM_BUDGETS = 50;
    const int NUM_TRANSACTIONS = 1000;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // 创建大量预算
    for (int i = 0; i < NUM_BUDGETS; i++) {
        Budget budget("B-" + std::to_string(i), 
                     "分类" + std::to_string(i),
                     10000.0 * (i + 1),  // 不同金额
                     BudgetPeriod::MONTHLY,
                     0.8 + (i * 0.01));  // 不同阈值
        ledgerManager->importBudget(budget);
    }
    
    // 创建大量交易
    double totalIncome = 0.0;
    double totalExpense = 0.0;
    
    for (int i = 0; i < NUM_TRANSACTIONS; i++) {
        bool isIncome = (i % 5 == 0);  // 每5笔交易有1笔收入
        std::string category = "分类" + std::to_string(i % NUM_BUDGETS);
        double amount = 100.0 + (i * 10.0);
        
        Transaction trans("T-" + std::to_string(i),
                         amount,
                         isIncome ? TransactionType::INCOME : TransactionType::EXPENSE,
                         "交易" + std::to_string(i),
                         category,
                         "ACC001");
        
        ledgerManager->importTransaction(trans);
        
        if (isIncome) {
            totalIncome += amount;
        } else {
            totalExpense += amount;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // 验证数据正确性
    EXPECT_NEAR(ledgerManager->getTotalIncome(), totalIncome, 0.01);
    EXPECT_NEAR(ledgerManager->getTotalExpense(), totalExpense, 0.01);
    EXPECT_EQ(ledgerManager->getBudgetCount(), NUM_BUDGETS);
    EXPECT_EQ(ledgerManager->getTransactionCount(), NUM_TRANSACTIONS);
    
    // 输出性能信息（测试报告中会显示）
    std::cout << "\n性能测试结果：" << std::endl;
    std::cout << "处理 " << NUM_BUDGETS << " 个预算和 " << NUM_TRANSACTIONS 
              << " 笔交易耗时: " << duration.count() << " 毫秒" << std::endl;
    std::cout << "平均每笔交易: " << (duration.count() / (double)NUM_TRANSACTIONS) 
              << " 毫秒" << std::endl;
    
    // 性能断言：处理1000笔交易应在5秒内完成
    EXPECT_LT(duration.count(), 5000);
}

// 测试9: 用户场景模拟
TEST_F(IntegrationTest, UserScenarioSimulation) {
    // 模拟一个月的家庭记账场景
    
    // 月初：设置预算
    std::cout << "\n=== 模拟家庭记账场景 ===" << std::endl;
    std::cout << "1. 月初设置预算..." << std::endl;
    
    ledgerManager->importBudget(Budget("B-rent", "房租", 2000, BudgetPeriod::MONTHLY, 0.95));
    ledgerManager->importBudget(Budget("B-food", "餐饮", 1500, BudgetPeriod::MONTHLY, 0.85));
    ledgerManager->importBudget(Budget("B-transport", "交通", 800, BudgetPeriod::MONTHLY, 0.9));
    ledgerManager->importBudget(Budget("B-entertainment", "娱乐", 1000, BudgetPeriod::MONTHLY, 0.8));
    
    // 5号：发工资
    std::cout << "2. 5号：收到工资..." << std::endl;
    ledgerManager->importTransaction(
        Transaction("T-salary", 8000, TransactionType::INCOME, "工资收入", "工资", "ACC001"));
    
    // 10号：支付房租
    std::cout << "3. 10号：支付房租..." << std::endl;
    ledgerManager->importTransaction(
        Transaction("T-rent", 2000, TransactionType::EXPENSE, "房租", "房租", "ACC001"));
    
    // 日常消费（模拟15笔消费）
    std::cout << "4. 日常消费记录..." << std::endl;
    double dailyExpenses = 0;
    for (int i = 1; i <= 15; i++) {
        double amount = 30 + (rand() % 70);  // 30-100元
        std::string category = (i % 3 == 0) ? "餐饮" : 
                              (i % 3 == 1) ? "交通" : "娱乐";
        
        ledgerManager->importTransaction(
            Transaction("T-day" + std::to_string(i), amount, 
                       TransactionType::EXPENSE, "日常消费" + std::to_string(i),
                       category, "ACC001"));
        dailyExpenses += amount;
    }
    
    // 25号：大额消费
    std::cout << "5. 25号：大额消费..." << std::endl;
    ledgerManager->importTransaction(
        Transaction("T-shopping", 800, TransactionType::EXPENSE, "购物", "娱乐", "ACC001"));
    
    // 月底：查看财务状况
    std::cout << "6. 月底：查看财务状况..." << std::endl;
    
    // 验证场景结果
    EXPECT_NEAR(ledgerManager->getTotalIncome(), 8000, 0.01);
    EXPECT_NEAR(ledgerManager->getTotalExpense(), 2000 + dailyExpenses + 800, 0.01);
    
    // 检查预警
    auto alerts = ledgerManager->getAllAlerts();
    std::cout << "7. 预警数量: " << alerts.size() << std::endl;
    
    // 这个测试主要验证流程，不设具体断言
    SUCCEED();
}