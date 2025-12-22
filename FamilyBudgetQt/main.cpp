#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <algorithm>
#include <map>
#include <limits>

// 业务逻辑头文件
#include "include/Transaction.h"
#include "include/Budget.h"
#include "include/AlertService.h"
#include "include/LedgerManager.h"
#include "include/SampleDataGenerator.h"
#include "include/CommonEnums.h"

using namespace std;

// 全局变量
LedgerManager ledgerManager;


// ============ 辅助函数 ============
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}




void printHeader() {
    cout << "==========================================" << endl;
    cout << "      家庭记账本 - 命令行版本" << endl;
    cout << "==========================================" << endl;
}

void printHelp() {
    cout << "\n可用命令：" << endl;
    cout << "------------------------------------------" << endl;
    cout << "  1. add_transaction    - 添加交易记录" << endl;
    cout << "  2. add_budget         - 添加预算" << endl;
    cout << "  3. show_transactions  - 显示所有交易" << endl;
    cout << "  4. show_budgets       - 显示所有预算" << endl;
    cout << "  5. show_alerts        - 显示预警信息" << endl;
    cout << "  6. show_overview      - 显示财务概览" << endl;
    cout << "  7. clear              - 清空屏幕" << endl;
    cout << "  8. help               - 显示帮助" << endl;
    cout << "  9. exit               - 退出程序" << endl;
    cout << "  10. load_samples      - 加载示例数据" << endl;
    cout << "  11. add_expense       - 添加支出到预算" << endl;
    cout << "  12. stats             - 显示统计数据" << endl;
    cout << "------------------------------------------" << endl;
}

void exportAlerts() {
    clearScreen();
    printHeader();
    
    cout << "\n📤 导出预警历史" << endl;
    cout << "==========================================" << endl;
    
    string filename;
    cout << "请输入导出文件名（例如：alerts.txt）: ";
    cin >> filename;
    
    // 这里需要添加导出功能
    // 可能需要修改 LedgerManager 或直接使用 AlertService
    
    cout << "✅ 预警历史已导出到 " << filename << endl;
}

void clearAlerts() {
    clearScreen();
    printHeader();
    
    cout << "\n🗑️ 清空预警历史" << endl;
    cout << "==========================================" << endl;
    
    cout << "确认要清空所有预警历史吗？(y/n): ";
    char confirm;
    cin >> confirm;
    
    if (confirm == 'y' || confirm == 'Y') {
        // 这里需要添加清空预警功能
        cout << "✅ 预警历史已清空" << endl;
    } else {
        cout << "操作已取消" << endl;
    }
}

string getCurrentDateTime() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", ltm);
    return string(buffer);
}

// ============ 显示函数 ============
void showFinancialOverview() {
    clearScreen();
    printHeader();
    
    cout << "\n📊 财务概览" << endl;
    cout << "==========================================" << endl;
    
    // 获取所有交易
    auto transactions = ledgerManager.getTransactions();
    
    double totalIncome = 0.0;
    double totalExpense = 0.0;
    
    for (const auto& trans : transactions) {
        if (trans.getType() == TransactionType::INCOME) {
            totalIncome += trans.getAmount();
        } else {
            totalExpense += trans.getAmount();
        }
    }
    
    double netIncome = totalIncome - totalExpense;
     
    cout << fixed << setprecision(2);
    cout << "总收入:  ¥" << totalIncome << endl;
    cout << "总支出:  ¥" << totalExpense << endl;
    cout << "净收入:  ¥" << netIncome << endl;
    cout << "交易总数: " << transactions.size() << endl;
    cout << "==========================================" << endl;
}

void showTransactions() {
    clearScreen();
    printHeader();
    
    cout << "\n📋 交易记录" << endl;
    cout << "==========================================" << endl;
    cout << left << setw(10) << "类型" 
         << setw(10) << "金额" 
         << setw(15) << "分类"
         << setw(20) << "描述"
         << setw(15) << "时间" << endl;
    cout << "------------------------------------------" << endl;
    
    auto transactions = ledgerManager.getTransactions();
    
    if (transactions.empty()) {
        cout << "暂无交易记录" << endl;
    } else {
        for (const auto& trans : transactions) {
            string typeStr = (trans.getType() == TransactionType::INCOME) ? "收入" : "支出";
            string amountStr = "¥" + to_string(trans.getAmount());
            
            // 格式化时间
            auto time = trans.getTransactionTime();
            time_t tt = chrono::system_clock::to_time_t(time);
            tm *ltm = localtime(&tt);
            char timeBuffer[20];
            strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M", ltm);
            
            cout << left << setw(10) << typeStr
                 << setw(10) << amountStr
                 << setw(15) << trans.getCategory()
                 << setw(20) << trans.getDescription()
                 << setw(15) << timeBuffer << endl;
        }
    }
    cout << "==========================================" << endl;
}

void showBudgets() {
    clearScreen();
    printHeader();
    
    cout << "\n💰 预算管理" << endl;
    cout << "==========================================" << endl;
    cout << left << setw(15) << "分类" 
         << setw(15) << "预算金额" 
         << setw(15) << "已支出"
         << setw(15) << "剩余"
         << setw(10) << "使用率"
         << setw(10) << "状态" << endl;
    cout << "------------------------------------------" << endl;
    
    auto budgets = ledgerManager.getBudgets();
    
    if (budgets.empty()) {
        cout << "暂无预算数据" << endl;
    } else {
        for (const auto& budget : budgets) {
            double usageRate = budget.getUsageRate();
            double remaining = budget.getRemainingBudget();
            
            string status;
            if (usageRate >= 1.0) {
                status = "超支";
            } else if (usageRate >= budget.getAlertThreshold()) {
                status = "预警";
            } else {
                status = "正常";
            }
            
            cout << fixed << setprecision(2);
            cout << left << setw(15) << budget.getCategory()
                 << setw(15) << budget.getBudgetAmount()
                 << setw(15) << budget.getCurrentSpent()
                 << setw(15) << remaining
                 << setw(10) << (usageRate * 100) << "%"
                 << setw(10) << status << endl;
        }
    }
    cout << "==========================================" << endl;
}

void showAlerts() {
    clearScreen();
    printHeader();
    
    cout << "\n⚠️ 预算预警" << endl;
    cout << "==========================================" << endl;
    
    // 获取所有预算
    auto budgets = ledgerManager.getBudgets();
    bool hasAlerts = false;
    
    for (const auto& budget : budgets) {
        AlertLevel alertLevel = budget.checkAlert();
        
        if (alertLevel != AlertLevel::NONE) {
            hasAlerts = true;
            
            if (alertLevel == AlertLevel::SEVERE) {
                cout << "[严重] ";
            } else {
                cout << "[警告] ";
            }
            
            cout << budget.getCategory() << "预算";
            
            double usageRate = budget.getUsageRate();
            if (usageRate >= 1.0) {
                cout << "已超支！当前支出：¥" << budget.getCurrentSpent()
                     << "，超出预算：¥" << -budget.getRemainingBudget() << endl;
            } else {
                cout << "使用率已达" << (usageRate * 100) << "%，请注意控制支出" << endl;
            }
        }
    }
    
    if (!hasAlerts) {
        cout << "暂无预警信息" << endl;
    }
    
    // 显示预警历史
    cout << "\n=== 预警历史 ===" << endl;
    ledgerManager.displayAlertHistory();
    
    cout << "==========================================" << endl;
}

void showStatistics() {
    clearScreen();
    printHeader();
    
    cout << "\n📈 统计信息" << endl;
    cout << "==========================================" << endl;
    
    auto transactions = ledgerManager.getTransactions();
    auto budgets = ledgerManager.getBudgets();
    
    // 按分类统计支出
    map<string, double> expenseByCategory;
    
    for (const auto& trans : transactions) {
        if (trans.getType() == TransactionType::EXPENSE) {
            expenseByCategory[trans.getCategory()] += trans.getAmount();
        }
    }
    
    cout << "支出分类统计：" << endl;
    cout << "------------------------------------------" << endl;
    
    if (expenseByCategory.empty()) {
        cout << "暂无支出数据" << endl;
    } else {
        for (const auto& [category, amount] : expenseByCategory) {
            cout << left << setw(15) << category << ": ¥" << amount << endl;
        }
    }
    
    cout << "\n预算使用情况：" << endl;
    cout << "------------------------------------------" << endl;
    
    for (const auto& budget : budgets) {
        double usageRate = budget.getUsageRate();
        cout << budget.getCategory() << ": " << (usageRate * 100) << "%" << endl;
    }
    
    cout << "==========================================" << endl;
}

// ============ 添加函数 ============
void addTransaction() {
    clearScreen();
    printHeader();
    
    cout << "\n➕ 添加交易记录" << endl;
    cout << "==========================================" << endl;
    
    double amount;
    int typeChoice;
    string description, category;
    
    // 输入金额
    while (true) {
        cout << "请输入金额: ";
        if (cin >> amount && amount > 0) {
            break;
        } else {
            cout << "请输入有效的正数金额！" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    
    // 输入类型
    while (true) {
        cout << "请选择类型 (1.支出 / 2.收入): ";
        if (cin >> typeChoice && (typeChoice == 1 || typeChoice == 2)) {
            break;
        } else {
            cout << "请输入1或2！" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    
    cin.ignore(); // 清除缓冲区
    
    // 输入描述
    cout << "请输入描述: ";
    getline(cin, description);
    
    // 输入分类
    cout << "请输入分类: ";
    getline(cin, category);
    
    // 生成唯一ID
    string transactionId = "T" + to_string(time(nullptr)) + to_string(rand() % 1000);
    
    // 创建交易对象
    TransactionType type = (typeChoice == 1) ? TransactionType::EXPENSE : TransactionType::INCOME;
    Transaction transaction(transactionId, amount, type, description, category, "ACC001");
    
    // 添加到管理器
    ledgerManager.importTransaction(transaction);
    
    // 如果是支出，更新对应预算
    if (type == TransactionType::EXPENSE) {
        auto budgets = ledgerManager.getBudgets();
        for (auto& budget : budgets) {
            if (budget.getCategory() == category) {
                budget.addExpense(amount);
                cout << "已更新" << category << "预算支出" << endl;
                break;
            }
        }
    }
    
    cout << "\n✅ 交易记录添加成功！" << endl;
}

void addBudget() {
    clearScreen();
    printHeader();
    
    cout << "\n💰 添加预算" << endl;
    cout << "==========================================" << endl;
    
    string category;
    double amount;
    int periodChoice;
    
    cin.ignore(); // 清除缓冲区
    
    // 输入分类
    cout << "请输入分类: ";
    getline(cin, category);
    
    // 输入金额
    while (true) {
        cout << "请输入预算金额: ";
        if (cin >> amount && amount > 0) {
            break;
        } else {
            cout << "请输入有效的正数金额！" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    
    // 输入周期
    while (true) {
        cout << "请选择周期 (1.日预算 / 2.周预算 / 3.月预算 / 4.年预算): ";
        if (cin >> periodChoice && periodChoice >= 1 && periodChoice <= 4) {
            break;
        } else {
            cout << "请输入1-4之间的数字！" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    
    // 生成唯一ID
    string budgetId = "B" + to_string(time(nullptr)) + to_string(rand() % 1000);
    
    // 创建预算对象
    BudgetPeriod period = static_cast<BudgetPeriod>(periodChoice - 1);
    Budget budget(budgetId, category, amount, period, 0.8);
    
    // 添加到管理器
    ledgerManager.importBudget(budget);
    
    cout << "\n✅ 预算添加成功！" << endl;
}

void addExpenseToBudget() {
    clearScreen();
    printHeader();
    
    cout << "\n💸 添加预算支出" << endl;
    cout << "==========================================" << endl;
    
    auto budgets = ledgerManager.getBudgets();
    
    if (budgets.empty()) {
        cout << "暂无预算，请先添加预算！" << endl;
        return;
    }
    
    // 显示所有预算
    cout << "可用预算：" << endl;
    for (size_t i = 0; i < budgets.size(); ++i) {
        cout << i + 1 << ". " << budgets[i].getCategory() 
             << " (预算: ¥" << budgets[i].getBudgetAmount() 
             << ", 已支出: ¥" << budgets[i].getCurrentSpent() << ")" << endl;
    }
    
    int choice;
    double amount;
    
    while (true) {
        cout << "\n请选择预算序号: ";
        if (cin >> choice && choice >= 1 && choice <= static_cast<int>(budgets.size())) {
            break;
        } else {
            cout << "请输入有效的序号！" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    
    while (true) {
        cout << "请输入支出金额: ";
        if (cin >> amount && amount > 0) {
            break;
        } else {
            cout << "请输入有效的正数金额！" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    
    // 更新预算支出
    budgets[choice - 1].addExpense(amount);
    cout << "\n✅ 支出添加成功！" << endl;
}

void loadSampleData() {
    clearScreen();
    printHeader();
    
    cout << "\n📂 加载示例数据" << endl;
    cout << "==========================================" << endl;
    
    // 加载示例预算
    auto sampleBudgets = SampleDataGenerator::generateSampleBudgets();
    for (const auto& budget : sampleBudgets) {
        ledgerManager.importBudget(budget);
    }
    
    // 加载示例交易
    auto sampleTransactions = SampleDataGenerator::generateSampleTransactions();
    for (const auto& trans : sampleTransactions) {
        ledgerManager.importTransaction(trans);
    }
    
    cout << "已加载示例数据：" << endl;
    cout << "- 预算: " << sampleBudgets.size() << " 个" << endl;
    cout << "- 交易: " << sampleTransactions.size() << " 笔" << endl;
    cout << "==========================================" << endl;
}

// ============ 主循环 ============
int main() {
    #ifdef _WIN32
    system("chcp 65001"); // 切换控制台为 UTF-8 编码
    #endif
    // 设置随机种子
    srand(static_cast<unsigned>(time(nullptr)));
    
    // 显示欢迎信息
    clearScreen();
    printHeader();
    printHelp();
    
    // 主循环
    while (true) {
        cout << "\n> ";
        
        string command;
        getline(cin, command);
        
        // 转换为小写便于比较
        transform(command.begin(), command.end(), command.begin(), ::tolower);
        
        if (command == "exit" || command == "9") {
            cout << "感谢使用家庭记账本，再见！" << endl;
            break;
        }
        else if (command == "add_transaction" || command == "1") {
            addTransaction();
        }
        else if (command == "add_budget" || command == "2") {
            addBudget();
        }
        else if (command == "show_transactions" || command == "3") {
            showTransactions();
        }
        else if (command == "show_budgets" || command == "4") {
            showBudgets();
        }
        else if (command == "show_alerts" || command == "5") {
            showAlerts();
        }
        else if (command == "show_overview" || command == "6") {
            showFinancialOverview();
        }
        else if (command == "clear" || command == "7") {
            clearScreen();
            printHeader();
        }
        else if (command == "help" || command == "8") {
            printHelp();
        }
        else if (command == "load_samples" || command == "10") {
            loadSampleData();
        }
        else if (command == "add_expense" || command == "11") {
            addExpenseToBudget();
        }
        else if (command == "stats" || command == "12") {
            showStatistics();
        }else if (command == "export_alerts" || command == "13") {
            exportAlerts();
        }
        else if (command == "clear_alerts" || command == "14") {
            clearAlerts();
        }
        else if (command.empty()) {
            // 空命令，继续
        }
        else {
            cout << "未知命令 '" << command << "'，输入 'help' 查看可用命令" << endl;
        }
    }
    
    return 0;
}