#ifndef SAMPLE_DATA_GENERATOR_H
#define SAMPLE_DATA_GENERATOR_H

#include <vector>
#include "Transaction.h"
#include "Budget.h"

class SampleDataGenerator {
public:
    static std::vector<Transaction> generateSampleTransactions();
    static std::vector<Budget> generateSampleBudgets();
};

#endif // SAMPLE_DATA_GENERATOR_H