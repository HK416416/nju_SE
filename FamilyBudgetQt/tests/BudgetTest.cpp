#include <gtest/gtest.h>
#include <iostream>
#include "../include/Budget.h"
#include "../include/CommonEnums.h"


class BudgetTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 创建测试预算对象
        monthlyBudget = new Budget(
            "BGT001",
            "食品",
            1000.0,
            BudgetPeriod::MONTHLY,
            0.8  // 阈值80%
        );
        
        dailyBudget = new Budget(
            "BGT002",
            "交通",
            50.0,
            BudgetPeriod::DAILY,
            0.9  // 阈值90%
        );
        
        zeroBudget = new Budget(
            "BGT003",
            "娱乐",
            0.0,
            BudgetPeriod::WEEKLY,
            0.5
        );
    }
    
    void TearDown() override {
        delete monthlyBudget;
        delete dailyBudget;
        delete zeroBudget;
    }
    
    Budget* monthlyBudget;
    Budget* dailyBudget;
    Budget* zeroBudget;
};

// 测试1: 构造函数测试
TEST_F(BudgetTest, ConstructorValidInput) {
    EXPECT_NE(monthlyBudget, nullptr);
    EXPECT_NE(dailyBudget, nullptr);
}

// 测试2: Getter方法测试 - 预算ID
TEST_F(BudgetTest, GetBudgetId) {
    EXPECT_EQ(monthlyBudget->getBudgetId(), "BGT001");
    EXPECT_EQ(dailyBudget->getBudgetId(), "BGT002");
    EXPECT_EQ(zeroBudget->getBudgetId(), "BGT003");
}

// 测试3: Getter方法测试 - 分类
TEST_F(BudgetTest, GetCategory) {
    EXPECT_EQ(monthlyBudget->getCategory(), "食品");
    EXPECT_EQ(dailyBudget->getCategory(), "交通");
    EXPECT_EQ(zeroBudget->getCategory(), "娱乐");
}

// 测试4: Getter方法测试 - 预算金额
TEST_F(BudgetTest, GetBudgetAmount) {
    EXPECT_DOUBLE_EQ(monthlyBudget->getBudgetAmount(), 1000.0);
    EXPECT_DOUBLE_EQ(dailyBudget->getBudgetAmount(), 50.0);
    EXPECT_DOUBLE_EQ(zeroBudget->getBudgetAmount(), 0.0);
}

// 测试5: Getter方法测试 - 预算周期
TEST_F(BudgetTest, GetPeriod) {
    EXPECT_EQ(monthlyBudget->getPeriod(), BudgetPeriod::MONTHLY);
    EXPECT_EQ(dailyBudget->getPeriod(), BudgetPeriod::DAILY);
    EXPECT_EQ(zeroBudget->getPeriod(), BudgetPeriod::WEEKLY);
}

// 测试6: Getter方法测试 - 警报阈值
TEST_F(BudgetTest, GetAlertThreshold) {
    EXPECT_DOUBLE_EQ(monthlyBudget->getAlertThreshold(), 0.8);
    EXPECT_DOUBLE_EQ(dailyBudget->getAlertThreshold(), 0.9);
    EXPECT_DOUBLE_EQ(zeroBudget->getAlertThreshold(), 0.5);
}

// 测试7: 初始支出为0
TEST_F(BudgetTest, InitialCurrentSpentIsZero) {
    EXPECT_DOUBLE_EQ(monthlyBudget->getCurrentSpent(), 0.0);
    EXPECT_DOUBLE_EQ(dailyBudget->getCurrentSpent(), 0.0);
    EXPECT_DOUBLE_EQ(zeroBudget->getCurrentSpent(), 0.0);
}

// 测试8: 添加支出测试
TEST_F(BudgetTest, AddExpense) {
    monthlyBudget->addExpense(100.0);
    EXPECT_DOUBLE_EQ(monthlyBudget->getCurrentSpent(), 100.0);
    
    monthlyBudget->addExpense(200.0);
    EXPECT_DOUBLE_EQ(monthlyBudget->getCurrentSpent(), 300.0);
}

// 测试9: 使用率计算测试 - 正常情况
TEST_F(BudgetTest, GetUsageRateNormal) {
    monthlyBudget->addExpense(500.0);
    EXPECT_DOUBLE_EQ(monthlyBudget->getUsageRate(), 0.5);
    
    monthlyBudget->addExpense(300.0);
    EXPECT_DOUBLE_EQ(monthlyBudget->getUsageRate(), 0.8);
}

// 测试10: 使用率计算测试 - 预算为0的情况
TEST_F(BudgetTest, GetUsageRateZeroBudget) {
    EXPECT_DOUBLE_EQ(zeroBudget->getUsageRate(), 0.0);
    
    zeroBudget->addExpense(100.0);
    EXPECT_DOUBLE_EQ(zeroBudget->getUsageRate(), 0.0);
}

// 测试11: 警报检查测试 - 无警报
TEST_F(BudgetTest, CheckAlertNone) {
    monthlyBudget->addExpense(400.0);
    EXPECT_EQ(monthlyBudget->checkAlert(), AlertLevel::NONE);
}

// 测试12: 警报检查测试 - 轻度警报
TEST_F(BudgetTest, CheckAlertMild) {
    monthlyBudget->addExpense(800.0);
    EXPECT_EQ(monthlyBudget->checkAlert(), AlertLevel::MILD);
    
    monthlyBudget->addExpense(100.0);
    EXPECT_EQ(monthlyBudget->checkAlert(), AlertLevel::MILD);
}

// 测试13: 警报检查测试 - 严重警报
TEST_F(BudgetTest, CheckAlertSevere) {
    monthlyBudget->addExpense(1000.0);
    EXPECT_EQ(monthlyBudget->checkAlert(), AlertLevel::SEVERE);
    
    monthlyBudget->addExpense(100.0);
    EXPECT_EQ(monthlyBudget->checkAlert(), AlertLevel::SEVERE);
}

// 测试14: 剩余预算计算测试
TEST_F(BudgetTest, GetRemainingBudget) {
    monthlyBudget->addExpense(300.0);
    EXPECT_DOUBLE_EQ(monthlyBudget->getRemainingBudget(), 700.0);
    
    monthlyBudget->addExpense(500.0);
    EXPECT_DOUBLE_EQ(monthlyBudget->getRemainingBudget(), 200.0);
    
    monthlyBudget->addExpense(300.0);
    EXPECT_DOUBLE_EQ(monthlyBudget->getRemainingBudget(), -100.0);
}

// 测试15: 边界条件测试 - 预算为负数的构造函数
TEST(BudgetTest_EdgeCases, NegativeBudgetAmount) {
    Budget negativeBudget("NEG001", "测试", -1000.0, BudgetPeriod::MONTHLY);
    EXPECT_DOUBLE_EQ(negativeBudget.getBudgetAmount(), -1000.0);
    
    negativeBudget.addExpense(500.0);
    EXPECT_DOUBLE_EQ(negativeBudget.getUsageRate(), -0.5);
}

// 测试16: 边界条件测试 - 添加负支出
// TEST_F(BudgetTest, AddNegativeExpense) {
//     int sp=monthlyBudget->getCurrentSpent();
//     int bg=monthlyBudget->getBudgetAmount();
//     monthlyBudget->addExpense(-100.0);
//     EXPECT_DOUBLE_EQ(monthlyBudget->getCurrentSpent(), sp);
//     EXPECT_DOUBLE_EQ(monthlyBudget->getBudgetAmount(), bg+100);
// }

// 测试17: 显示功能测试
TEST_F(BudgetTest, DisplayDoesNotCrash) {
    EXPECT_NO_THROW(monthlyBudget->display());
    EXPECT_NO_THROW(dailyBudget->display());
    EXPECT_NO_THROW(zeroBudget->display());
}

// 测试18: 阈值边界测试
TEST(BudgetTest_EdgeCases, AlertThresholdBoundary) {
    Budget budget("BGT004", "测试", 1000.0, BudgetPeriod::MONTHLY, 0.0);
    budget.addExpense(0.0);
    EXPECT_EQ(budget.checkAlert(), AlertLevel::MILD);
}

// 测试19: 默认构造函数测试
TEST(BudgetTest_EdgeCases, DefaultConstructor) {
    Budget defaultBudget;
    
    EXPECT_TRUE(defaultBudget.getBudgetId().empty());
    EXPECT_TRUE(defaultBudget.getCategory().empty());
    EXPECT_DOUBLE_EQ(defaultBudget.getBudgetAmount(), 0.0);
    EXPECT_EQ(defaultBudget.getPeriod(), BudgetPeriod::MONTHLY);
    EXPECT_DOUBLE_EQ(defaultBudget.getCurrentSpent(), 0.0);
}

// 测试20: 连续添加支出测试
TEST(BudgetTest_EdgeCases, MultipleExpenseAdditions) {
    Budget budget("MULTI001", "测试", 100.0, BudgetPeriod::DAILY);
    
    for(int i = 1; i <= 10; i++) {
        budget.addExpense(5.0);
        EXPECT_DOUBLE_EQ(budget.getCurrentSpent(), i * 5.0);
    }
    
    EXPECT_DOUBLE_EQ(budget.getCurrentSpent(), 50.0);
    EXPECT_DOUBLE_EQ(budget.getUsageRate(), 0.5);
}

// 测试21: 精确计算测试
TEST(BudgetTest_EdgeCases, PreciseCalculations) {
    Budget budget("PREC001", "测试", 1000.0, BudgetPeriod::MONTHLY, 0.75);
    
    // 添加精确的小数支出
    budget.addExpense(333.33);
    budget.addExpense(333.33);
    budget.addExpense(333.34);
    
    EXPECT_NEAR(budget.getCurrentSpent(), 1000.0, 0.01);
    EXPECT_NEAR(budget.getUsageRate(), 1.0, 0.0001);
    EXPECT_EQ(budget.checkAlert(), AlertLevel::SEVERE);
}

// 测试22: 不同预算周期测试
TEST(BudgetTest_EdgeCases, DifferentPeriods) {
    Budget daily("PERIOD001", "测试", 100.0, BudgetPeriod::DAILY);
    Budget weekly("PERIOD002", "测试", 500.0, BudgetPeriod::WEEKLY);
    Budget monthly("PERIOD003", "测试", 2000.0, BudgetPeriod::MONTHLY);
    Budget yearly("PERIOD004", "测试", 24000.0, BudgetPeriod::YEARLY);
    
    EXPECT_EQ(daily.getPeriod(), BudgetPeriod::DAILY);
    EXPECT_EQ(weekly.getPeriod(), BudgetPeriod::WEEKLY);
    EXPECT_EQ(monthly.getPeriod(), BudgetPeriod::MONTHLY);
    EXPECT_EQ(yearly.getPeriod(), BudgetPeriod::YEARLY);
}