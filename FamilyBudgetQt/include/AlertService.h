#ifndef ALERT_SERVICE_H
#define ALERT_SERVICE_H

#include <vector>
#include <string>
#include <chrono>
#include "Budget.h"

class AlertService {
private:
    std::vector<std::string> alertHistory;

public:
    // 构造函数
    AlertService();
    
    // 预警检查方法
    void checkBudgetAlert(const Budget& budget);
    void sendAlert(const std::string& message, AlertLevel level);
    
    // 预警历史管理
    void recordAlertHistory(const std::string& alertMsg);
    void displayAlertHistory() const;
    
    // ============ 新增方法 ============
    // 手动添加预警
    void addAlert(const std::string& alert);
    
    // 清空预警历史
    void clearAlerts();
    
    // 获取预警历史
    std::vector<std::string> getAlertHistory() const;
    
    // 获取预警数量
    size_t getAlertCount() const;
    
    // 获取最近的预警（最近N条）
    std::vector<std::string> getRecentAlerts(size_t count = 5) const;
    
    // 检查是否有预警
    bool hasAlerts() const;
    
    // 获取指定类型的预警
    std::vector<std::string> getAlertsByLevel(AlertLevel level) const;
    
    // 导出预警历史到文件
    bool exportAlertHistory(const std::string& filename) const;
    
    // 从文件导入预警历史
    bool importAlertHistory(const std::string& filename);

private:
    // 生成预警消息
    std::string generateAlertMessage(const Budget& budget, AlertLevel level);
    
    // 获取当前时间戳字符串
    std::string getCurrentTimestamp() const;
    
    // 从预警消息中提取预警级别
    AlertLevel extractAlertLevel(const std::string& alertMessage) const;
};

#endif // ALERT_SERVICE_H