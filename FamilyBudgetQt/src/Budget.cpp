#include "../include/Budget.h"
#include <iostream>

Budget::Budget(const std::string& id, const std::string& cat, double amount, 
               BudgetPeriod p, double threshold)
    : budgetId(id), category(cat), budgetAmount(amount), 
      period(p), alertThreshold(threshold), currentSpent(0) {}

std::string Budget::getBudgetId() const { return budgetId; }
std::string Budget::getCategory() const { return category; }
double Budget::getBudgetAmount() const { return budgetAmount; }
BudgetPeriod Budget::getPeriod() const { return period; }
double Budget::getAlertThreshold() const { return alertThreshold; }
double Budget::getCurrentSpent() const { return currentSpent; }

void Budget::addExpense(double amount) {
    currentSpent += amount;
}

double Budget::getUsageRate() const {
    if (budgetAmount == 0) return 0;
    return currentSpent / budgetAmount;
}

AlertLevel Budget::checkAlert() const {
    double usageRate = getUsageRate();
    if (usageRate >= 1.0) {
        return AlertLevel::SEVERE;
    } else if (usageRate >= alertThreshold) {
        return AlertLevel::MILD;
    }
    return AlertLevel::NONE;
}

double Budget::getRemainingBudget() const {
    return budgetAmount - currentSpent;
}

void Budget::display() const {
    std::string periodStr;
    switch (period) {
        case BudgetPeriod::DAILY: periodStr = "日预算"; break;
        case BudgetPeriod::WEEKLY: periodStr = "周预算"; break;
        case BudgetPeriod::MONTHLY: periodStr = "月预算"; break;
        case BudgetPeriod::YEARLY: periodStr = "年预算"; break;
    }
    
    std::cout << "预算ID: " << budgetId 
              << " | 分类: " << category 
              << " | " << periodStr << ": " << budgetAmount
              << " | 已支出: " << currentSpent 
              << " | 使用率: " << (getUsageRate() * 100) << "%" << std::endl;
}