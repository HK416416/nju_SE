#include "../include/AlertService.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#endif

// ============ 构造函数 ============
AlertService::AlertService() {
    // 构造函数为空
}

// ============ 预警检查方法 ============
void AlertService::checkBudgetAlert(const Budget& budget) {
    AlertLevel level = budget.checkAlert();
    
    if (level != AlertLevel::NONE) {
        std::string alertMsg = generateAlertMessage(budget, level);
        sendAlert(alertMsg, level);
        recordAlertHistory(alertMsg);
    }
}

void AlertService::sendAlert(const std::string& message, AlertLevel level) {
    std::string levelStr;
    std::string colorCode;
    
    switch (level) {
        case AlertLevel::MILD:
            levelStr = "轻度预警";
            colorCode = "\033[33m";  // 黄色
            break;
        case AlertLevel::SEVERE:
            levelStr = "重度预警";
            colorCode = "\033[31m";  // 红色
            break;
        default:
            levelStr = "预警";
            colorCode = "\033[0m";   // 默认
            break;
    }
    
#ifdef _WIN32
    // Windows 控制台颜色设置
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (level == AlertLevel::SEVERE) {
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
    } else if (level == AlertLevel::MILD) {
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    }
#endif
    
    std::cout << "\n" << colorCode << "=== " << levelStr << " ===" << "\033[0m" << std::endl;
    std::cout << colorCode << message << "\033[0m" << std::endl;
    std::cout << colorCode << "==================" << "\033[0m" << std::endl;
    
#ifdef _WIN32
    // 恢复控制台颜色
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif
}

std::string AlertService::generateAlertMessage(const Budget& budget, AlertLevel level) {
    std::string periodStr;
    switch (budget.getPeriod()) {
        case BudgetPeriod::DAILY: periodStr = "日"; break;
        case BudgetPeriod::WEEKLY: periodStr = "周"; break;
        case BudgetPeriod::MONTHLY: periodStr = "月"; break;
        case BudgetPeriod::YEARLY: periodStr = "年"; break;
    }

    std::string message;
    if (level == AlertLevel::MILD) {
        message = "【预算提醒】" + budget.getCategory() + "分类" + periodStr + 
                 "预算使用率已达" + std::to_string(static_cast<int>(budget.getUsageRate() * 100)) + 
                 "%，请注意控制支出。剩余预算：" + std::to_string(budget.getRemainingBudget());
    } else {
        message = "【预算超支】" + budget.getCategory() + "分类" + periodStr + 
                 "预算已超支！当前支出：" + std::to_string(budget.getCurrentSpent()) + 
                 "，超出预算：" + std::to_string(-budget.getRemainingBudget());
    }
    
    return message;
}

// ============ 预警历史管理 ============
void AlertService::recordAlertHistory(const std::string& alertMsg) {
    std::string timestamp = getCurrentTimestamp();
    std::string fullMessage = timestamp + " - " + alertMsg;
    alertHistory.push_back(fullMessage);
    
    // 限制历史记录数量，最多保留100条
    const size_t MAX_HISTORY = 100;
    if (alertHistory.size() > MAX_HISTORY) {
        alertHistory.erase(alertHistory.begin());
    }
}

void AlertService::displayAlertHistory() const {
    std::cout << "\n=== 预警历史 ===" << std::endl;
    
    if (alertHistory.empty()) {
        std::cout << "暂无预警信息" << std::endl;
        return;
    }
    
    for (size_t i = 0; i < alertHistory.size(); ++i) {
        std::cout << i + 1 << ". " << alertHistory[i] << std::endl;
    }
}

// ============ 新增方法实现 ============
void AlertService::addAlert(const std::string& alert) {
    std::string timestamp = getCurrentTimestamp();
    std::string fullMessage = timestamp + " - " + alert;
    alertHistory.push_back(fullMessage);
    
    // 限制历史记录数量
    const size_t MAX_HISTORY = 100;
    if (alertHistory.size() > MAX_HISTORY) {
        alertHistory.erase(alertHistory.begin());
    }
}

void AlertService::clearAlerts() {
    alertHistory.clear();
}

std::vector<std::string> AlertService::getAlertHistory() const {
    return alertHistory;
}

size_t AlertService::getAlertCount() const {
    return alertHistory.size();
}

std::vector<std::string> AlertService::getRecentAlerts(size_t count) const {
    size_t startIdx = (alertHistory.size() > count) ? alertHistory.size() - count : 0;
    std::vector<std::string> recentAlerts;
    
    for (size_t i = startIdx; i < alertHistory.size(); ++i) {
        recentAlerts.push_back(alertHistory[i]);
    }
    
    return recentAlerts;
}

bool AlertService::hasAlerts() const {
    return !alertHistory.empty();
}

std::vector<std::string> AlertService::getAlertsByLevel(AlertLevel level) const {
    std::vector<std::string> filteredAlerts;
    std::string levelStr;
    
    switch (level) {
        case AlertLevel::MILD: levelStr = "轻度预警"; break;
        case AlertLevel::SEVERE: levelStr = "重度预警"; break;
        default: levelStr = "预警"; break;
    }
    
    for (const auto& alert : alertHistory) {
        if (alert.find(levelStr) != std::string::npos) {
            filteredAlerts.push_back(alert);
        }
    }
    
    return filteredAlerts;
}

bool AlertService::exportAlertHistory(const std::string& filename) const {
    std::ofstream outFile(filename);
    
    if (!outFile.is_open()) {
        std::cerr << "错误：无法打开文件 " << filename << " 进行写入" << std::endl;
        return false;
    }
    
    for (const auto& alert : alertHistory) {
        outFile << alert << std::endl;
    }
    
    outFile.close();
    return true;
}

bool AlertService::importAlertHistory(const std::string& filename) {
    std::ifstream inFile(filename);
    
    if (!inFile.is_open()) {
        std::cerr << "错误：无法打开文件 " << filename << " 进行读取" << std::endl;
        return false;
    }
    
    alertHistory.clear();
    std::string line;
    
    while (std::getline(inFile, line)) {
        if (!line.empty()) {
            alertHistory.push_back(line);
        }
    }
    
    inFile.close();
    return true;
}

// ============ 私有辅助方法 ============
std::string AlertService::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()) % 1000;
    
    std::tm* tm = std::localtime(&time);
    char buffer[80];
    
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm);
    
    std::ostringstream oss;
    oss << buffer << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    return oss.str();
}

AlertLevel AlertService::extractAlertLevel(const std::string& alertMessage) const {
    if (alertMessage.find("重度预警") != std::string::npos) {
        return AlertLevel::SEVERE;
    } else if (alertMessage.find("轻度预警") != std::string::npos) {
        return AlertLevel::MILD;
    } else {
        return AlertLevel::NONE;
    }
}