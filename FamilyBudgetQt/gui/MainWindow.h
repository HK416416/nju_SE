#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDate>

// 包含原有的业务逻辑头文件
#include "../include/Transaction.h"
#include "../include/Budget.h"
#include "../include/AlertService.h"
#include "../include/LedgerManager.h"
#include "../include/SampleDataGenerator.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddTransaction();
    void onAddBudget();
    void onRefresh();
    void onTabChanged(int index);

private:
    void setupUI();
    void setupConnections();
    void updateFinancialOverview();
    void updateBudgetTable();
    void updateTransactionTable();
    void updateAlerts();
    
    // UI 组件
    QTabWidget *tabWidget;
    QTableWidget *budgetTable;
    QTableWidget *transactionTable;
    QListWidget *alertList;
    
    QLabel *totalIncomeLabel;
    QLabel *totalExpenseLabel;
    QLabel *netIncomeLabel;
    QLabel *transactionCountLabel;
    
    QPushButton *addTransactionBtn;
    QPushButton *addBudgetBtn;
    QPushButton *refreshBtn;
    
    // 业务逻辑
    LedgerManager ledgerManager;
};

#endif // MAINWINDOW_H