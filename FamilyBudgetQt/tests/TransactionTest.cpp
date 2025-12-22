#include <gtest/gtest.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <cmath>
#include <limits>
#include "../include/Transaction.h"
#include "../include/CommonEnums.h"

class TransactionTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 测试数据
        incomeTransaction = new Transaction(
            "INC001",
            1000.50,
            TransactionType::INCOME,
            "工资收入",
            "工资",
            "ACC001"
        );
        
        expenseTransaction = new Transaction(
            "EXP001",
            500.75,
            TransactionType::EXPENSE,
            "超市购物",
            "食品",
            "ACC001"
        );
        
        edgeCaseTransaction = new Transaction(
            "EDG001",
            0.0,
            TransactionType::EXPENSE,
            "",
            "",
            ""
        );
    }
    
    void TearDown() override {
        delete incomeTransaction;
        delete expenseTransaction;
        delete edgeCaseTransaction;
    }
    
    Transaction* incomeTransaction;
    Transaction* expenseTransaction;
    Transaction* edgeCaseTransaction;
};
//测试0: 
TEST_F(TransactionTest, DummyTest) {
    EXPECT_EQ(0,0);
}
// 测试1: 构造函数正常情况
TEST_F(TransactionTest, ConstructorValidInput) {
    EXPECT_NE(incomeTransaction, nullptr);
    EXPECT_NE(expenseTransaction, nullptr);
}

// 测试2: Getter方法测试 - 记录ID
TEST_F(TransactionTest, GetRecordId) {
    EXPECT_EQ(incomeTransaction->getRecordId(), "INC001");
    EXPECT_EQ(expenseTransaction->getRecordId(), "EXP001");
}

// 测试3: Getter方法测试 - 金额
TEST_F(TransactionTest, GetAmount) {
    EXPECT_DOUBLE_EQ(incomeTransaction->getAmount(), 1000.50);
    EXPECT_DOUBLE_EQ(expenseTransaction->getAmount(), 500.75);
    EXPECT_DOUBLE_EQ(edgeCaseTransaction->getAmount(), 0.0);
}

// 测试4: Getter方法测试 - 交易类型
TEST_F(TransactionTest, GetType) {
    EXPECT_EQ(incomeTransaction->getType(), TransactionType::INCOME);
    EXPECT_EQ(expenseTransaction->getType(), TransactionType::EXPENSE);
}

// 测试5: Getter方法测试 - 描述
TEST_F(TransactionTest, GetDescription) {
    EXPECT_EQ(incomeTransaction->getDescription(), "工资收入");
    EXPECT_EQ(expenseTransaction->getDescription(), "超市购物");
    EXPECT_TRUE(edgeCaseTransaction->getDescription().empty());
}

// 测试6: Getter方法测试 - 分类
TEST_F(TransactionTest, GetCategory) {
    EXPECT_EQ(incomeTransaction->getCategory(), "工资");
    EXPECT_EQ(expenseTransaction->getCategory(), "食品");
    EXPECT_TRUE(edgeCaseTransaction->getCategory().empty());
}

// 测试7: Getter方法测试 - 账户ID
TEST_F(TransactionTest, GetAccountId) {
    EXPECT_EQ(incomeTransaction->getAccountId(), "ACC001");
    EXPECT_EQ(expenseTransaction->getAccountId(), "ACC001");
    EXPECT_TRUE(edgeCaseTransaction->getAccountId().empty());
}

// 测试8: 交易时间测试
TEST_F(TransactionTest, TransactionTimeIsRecent) {
    auto now = std::chrono::system_clock::now();
    auto transTime = incomeTransaction->getTransactionTime();
    
    // 检查交易时间不应晚于当前时间
    EXPECT_LE(transTime, now);
    
    // 检查交易时间应在合理范围内（不超过5秒前，考虑测试运行时间）
    auto diff = std::chrono::duration_cast<std::chrono::seconds>(now - transTime);
    EXPECT_LE(diff.count(), 5);
}

// 测试9: 显示功能测试（无法验证输出，但可以测试不崩溃）
TEST_F(TransactionTest, DisplayDoesNotCrash) {
    EXPECT_NO_THROW(incomeTransaction->display());
    EXPECT_NO_THROW(expenseTransaction->display());
    EXPECT_NO_THROW(edgeCaseTransaction->display());
}

// 测试10: 交易对象比较测试
TEST(TransactionTest_EdgeCases, TransactionComparison) {
    Transaction t1("T001", 100.0, TransactionType::INCOME, "收入1", "工资", "ACC1");
    Transaction t2("T002", 200.0, TransactionType::EXPENSE, "支出1", "购物", "ACC1");
    
    EXPECT_NE(t1.getRecordId(), t2.getRecordId());
    EXPECT_NE(t1.getAmount(), t2.getAmount());
    EXPECT_NE(t1.getType(), t2.getType());
}

// 测试11: 边界条件测试 - 负数金额
TEST(TransactionTest_EdgeCases, NegativeAmount) {
    Transaction negativeTrans("NEG001", -100.0, TransactionType::EXPENSE, 
                              "退款", "退货", "ACC001");
    EXPECT_DOUBLE_EQ(negativeTrans.getAmount(), -100.0);
}

// 测试12: 边界条件测试 - 大金额
// TEST(TransactionTest_EdgeCases, LargeAmount) {
//     Transaction largeTrans("LARGE001", 1e308 * 10, TransactionType::INCOME, 
//                            "大额转账", "投资", "ACC002");
//     double result=largeTrans.getAmount();
//     EXPECT_TRUE(!(std::isinf(result)));// 检查是否为无穷大
//     EXPECT_GT(result, 0);
// }

// 测试13: 边界条件测试 - 长字符串
TEST(TransactionTest_EdgeCases, LongStrings) {
    std::string longDesc(100, 'A');  // 100个'A'
    std::string longCategory(50, 'B');
    std::string longAccount(30, 'C');
    
    Transaction longTrans("LONG001", 100.0, TransactionType::EXPENSE, 
                          longDesc, longCategory, longAccount);
    
    EXPECT_EQ(longTrans.getDescription().length(), 100);
    EXPECT_EQ(longTrans.getCategory().length(), 50);
    EXPECT_EQ(longTrans.getAccountId().length(), 30);
}

// 测试14: 交易时间顺序测试
TEST(TransactionTest_EdgeCases, TransactionTimeOrder) {
    Transaction t1("T001", 100.0, TransactionType::INCOME, "收入1", "工资", "ACC1");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));  // 等待10毫秒
    Transaction t2("T002", 200.0, TransactionType::EXPENSE, "支出1", "购物", "ACC1");
    
    EXPECT_LT(t1.getTransactionTime(), t2.getTransactionTime());
}