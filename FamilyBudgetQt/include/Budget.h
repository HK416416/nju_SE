#ifndef BUDGET_H
#define BUDGET_H

#include <string>
#include "CommonEnums.h"

class Budget {
private:
    std::string budgetId;
    std::string category;
    double budgetAmount;
    BudgetPeriod period;
    double alertThreshold;
    double currentSpent;

public:
    Budget() : budgetId(""), category(""), budgetAmount(0.0), period(MONTHLY), currentSpent(0.0) {}

    Budget(const std::string& id, const std::string& cat, double amount, 
           BudgetPeriod p, double threshold = 0.8);

    // Getter 方法
    std::string getBudgetId() const;
    std::string getCategory() const;
    double getBudgetAmount() const;
    BudgetPeriod getPeriod() const;
    double getAlertThreshold() const;
    double getCurrentSpent() const;

    // 业务方法
    void addExpense(double amount);
    double getUsageRate() const;
    AlertLevel checkAlert() const;
    double getRemainingBudget() const;
    void display() const;
};

#endif // BUDGET_H