#ifndef COMMON_ENUMS_H
#define COMMON_ENUMS_H

// 枚举定义
enum TransactionType {
    INCOME,     // 收入
    EXPENSE     // 支出
};

enum AlertLevel {
    NONE,       // 无预警
    MILD,       // 轻度预警
    SEVERE      // 重度预警
};

enum BudgetPeriod {
    DAILY,      // 日预算
    WEEKLY,     // 周预算
    MONTHLY,    // 月预算
    YEARLY      // 年预算

};

#endif // COMMON_ENUMS_H