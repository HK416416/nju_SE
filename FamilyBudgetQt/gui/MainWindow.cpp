#include "MainWindow.h"
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    setupConnections();
    
    // 初始化示例数据
    auto sampleBudgets = SampleDataGenerator::generateSampleBudgets();
    ledgerManager.importBudgets(sampleBudgets);
    
    auto sampleTransactions = SampleDataGenerator::generateSampleTransactions();
    ledgerManager.importTransactions(sampleTransactions);
    
    updateFinancialOverview();
    updateBudgetTable();
    updateTransactionTable();
    updateAlerts();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    setWindowTitle("家庭记账本 - Qt版");
    setMinimumSize(1000, 700);
    resize(1200, 800);
    
    // 创建中心部件
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    // 创建选项卡
    tabWidget = new QTabWidget(this);
    
    // === 财务概览选项卡 ===
    QWidget *overviewTab = new QWidget();
    QVBoxLayout *overviewLayout = new QVBoxLayout(overviewTab);
    
    // 财务统计组
    QGroupBox *statsGroup = new QGroupBox("📊 财务统计");
    statsGroup->setStyleSheet("QGroupBox { font-weight: bold; }");
    QGridLayout *statsLayout = new QGridLayout(statsGroup);
    
    statsLayout->addWidget(new QLabel("总收入:"), 0, 0);
    totalIncomeLabel = new QLabel("¥0.00");
    totalIncomeLabel->setStyleSheet("color: #27ae60; font-size: 18px; font-weight: bold;");
    statsLayout->addWidget(totalIncomeLabel, 0, 1);
    
    statsLayout->addWidget(new QLabel("总支出:"), 1, 0);
    totalExpenseLabel = new QLabel("¥0.00");
    totalExpenseLabel->setStyleSheet("color: #e74c3c; font-size: 18px; font-weight: bold;");
    statsLayout->addWidget(totalExpenseLabel, 1, 1);
    
    statsLayout->addWidget(new QLabel("净收入:"), 2, 0);
    netIncomeLabel = new QLabel("¥0.00");
    netIncomeLabel->setStyleSheet("color: #3498db; font-size: 18px; font-weight: bold;");
    statsLayout->addWidget(netIncomeLabel, 2, 1);
    
    statsLayout->addWidget(new QLabel("交易总数:"), 0, 2);
    transactionCountLabel = new QLabel("0");
    transactionCountLabel->setStyleSheet("font-size: 18px; font-weight: bold;");
    statsLayout->addWidget(transactionCountLabel, 0, 3);
    
    // 预警信息组
    QGroupBox *alertGroup = new QGroupBox("⚠️ 预算预警");
    alertGroup->setStyleSheet("QGroupBox { font-weight: bold; }");
    QVBoxLayout *alertLayout = new QVBoxLayout(alertGroup);
    alertList = new QListWidget();
    alertList->setStyleSheet("QListWidget { font-size: 11px; }");
    alertLayout->addWidget(alertList);
    
    // 按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    addTransactionBtn = new QPushButton("➕ 添加交易");
    addTransactionBtn->setStyleSheet("QPushButton { padding: 8px; font-weight: bold; }");
    
    addBudgetBtn = new QPushButton("💰 添加预算");
    addBudgetBtn->setStyleSheet("QPushButton { padding: 8px; font-weight: bold; }");
    
    refreshBtn = new QPushButton("🔄 刷新");
    refreshBtn->setStyleSheet("QPushButton { padding: 8px; }");
    
    buttonLayout->addWidget(addTransactionBtn);
    buttonLayout->addWidget(addBudgetBtn);
    buttonLayout->addWidget(refreshBtn);
    buttonLayout->addStretch();
    
    overviewLayout->addWidget(statsGroup);
    overviewLayout->addWidget(alertGroup);
    overviewLayout->addLayout(buttonLayout);
    
    // === 预算管理选项卡 ===
    QWidget *budgetTab = new QWidget();
    QVBoxLayout *budgetLayout = new QVBoxLayout(budgetTab);
    
    budgetTable = new QTableWidget();
    budgetTable->setColumnCount(6);
    budgetTable->setHorizontalHeaderLabels({"分类", "预算金额", "已支出", "剩余", "使用率", "状态"});
    budgetTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    budgetTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    budgetTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    budgetLayout->addWidget(budgetTable);
    
    // === 交易记录选项卡 ===
    QWidget *transactionTab = new QWidget();
    QVBoxLayout *transactionLayout = new QVBoxLayout(transactionTab);
    
    transactionTable = new QTableWidget();
    transactionTable->setColumnCount(6);
    transactionTable->setHorizontalHeaderLabels({"日期", "时间", "类型", "金额", "分类", "描述"});
    transactionTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    transactionTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    transactionTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    transactionLayout->addWidget(transactionTable);
    
    // 添加选项卡
    tabWidget->addTab(overviewTab, "🏠 财务概览");
    tabWidget->addTab(budgetTab, "💰 预算管理");
    tabWidget->addTab(transactionTab, "📋 交易记录");
    
    mainLayout->addWidget(tabWidget);
}

void MainWindow::setupConnections()
{
    connect(addTransactionBtn, &QPushButton::clicked, this, &MainWindow::onAddTransaction);
    connect(addBudgetBtn, &QPushButton::clicked, this, &MainWindow::onAddBudget);
    connect(refreshBtn, &QPushButton::clicked, this, &MainWindow::onRefresh);
    connect(tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
}

void MainWindow::onAddTransaction()
{
    QDialog dialog(this);
    dialog.setWindowTitle("添加交易记录");
    dialog.setFixedSize(400, 300);
    
    QFormLayout *form = new QFormLayout(&dialog);
    
    QLineEdit *amountInput = new QLineEdit();
    amountInput->setPlaceholderText("输入金额");
    
    QComboBox *typeCombo = new QComboBox();
    typeCombo->addItems({"支出", "收入"});
    
    QComboBox *categoryCombo = new QComboBox();
    categoryCombo->addItems({"餐饮", "交通", "购物", "娱乐", "工资", "其他"});
    
    QLineEdit *descInput = new QLineEdit();
    descInput->setPlaceholderText("交易描述");
    
    form->addRow("金额:", amountInput);
    form->addRow("类型:", typeCombo);
    form->addRow("分类:", categoryCombo);
    form->addRow("描述:", descInput);
    
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    form->addRow(buttons);
    
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    if (dialog.exec() == QDialog::Accepted) {
        bool ok;
        double amount = amountInput->text().toDouble(&ok);
        
        if (ok && amount > 0) {
            TransactionType type = (typeCombo->currentIndex() == 0) ? TransactionType::EXPENSE : TransactionType::INCOME;
            QString category = categoryCombo->currentText();
            QString description = descInput->text();
            
            // 生成唯一ID
            QString transactionId = "T" + QString::number(QDateTime::currentMSecsSinceEpoch());
            
            Transaction transaction(transactionId.toStdString(), amount, type, 
                                   description.toStdString(), category.toStdString(), "ACC001");
            ledgerManager.importTransaction(transaction);
            
            QMessageBox::information(this, "成功", "交易记录添加成功！");
            onRefresh();
        } else {
            QMessageBox::warning(this, "错误", "请输入有效的金额！");
        }
    }
}

void MainWindow::onAddBudget()
{
    QDialog dialog(this);
    dialog.setWindowTitle("添加预算");
    dialog.setFixedSize(350, 200);
    
    QFormLayout *form = new QFormLayout(&dialog);
    
    QComboBox *categoryCombo = new QComboBox();
    categoryCombo->addItems({"餐饮", "交通", "购物", "娱乐", "其他"});
    
    QLineEdit *amountInput = new QLineEdit();
    amountInput->setPlaceholderText("预算金额");
    
    QComboBox *periodCombo = new QComboBox();
    periodCombo->addItems({"日预算", "周预算", "月预算", "年预算"});
    
    form->addRow("分类:", categoryCombo);
    form->addRow("金额:", amountInput);
    form->addRow("周期:", periodCombo);
    
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    form->addRow(buttons);
    
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    if (dialog.exec() == QDialog::Accepted) {
        bool ok;
        double amount = amountInput->text().toDouble(&ok);
        
        if (ok && amount > 0) {
            QString category = categoryCombo->currentText();
            BudgetPeriod period = static_cast<BudgetPeriod>(periodCombo->currentIndex());
            
            // 生成唯一ID
            QString budgetId = "B" + QString::number(QDateTime::currentMSecsSinceEpoch());
            
            Budget budget(budgetId.toStdString(), category.toStdString(), amount, period, 0.8);
            ledgerManager.importBudget(budget);
            
            QMessageBox::information(this, "成功", "预算添加成功！");
            onRefresh();
        } else {
            QMessageBox::warning(this, "错误", "请输入有效的金额！");
        }
    }
}

void MainWindow::onRefresh()
{
    updateFinancialOverview();
    updateBudgetTable();
    updateTransactionTable();
    updateAlerts();
}

void MainWindow::onTabChanged(int index)
{
    // 切换标签时自动刷新
    onRefresh();
}

void MainWindow::updateFinancialOverview()
{
    // 这里需要从 LedgerManager 获取实际数据
    // 暂时使用固定值演示
    totalIncomeLabel->setText("¥5,000.00");
    totalExpenseLabel->setText("¥730.00");
    netIncomeLabel->setText("¥4,270.00");
    transactionCountLabel->setText("5");
}

void MainWindow::updateBudgetTable()
{
    budgetTable->setRowCount(4);
    
    // 示例数据
    QStringList budgetData = {
        "餐饮", "¥500.00", "¥150.00", "¥350.00", "30%", "正常",
        "交通", "¥300.00", "¥80.00", "¥220.00", "27%", "正常", 
        "购物", "¥600.00", "¥300.00", "¥300.00", "50%", "预警",
        "娱乐", "¥200.00", "¥200.00", "¥0.00", "100%", "超支"
    };
    
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 6; ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(budgetData[row * 6 + col]);
            
            // 设置颜色
            if (col == 5) {
                if (budgetData[row * 6 + col] == "超支") {
                    item->setForeground(Qt::red);
                    item->setBackground(QColor(255, 200, 200));
                } else if (budgetData[row * 6 + col] == "预警") {
                    item->setForeground(QColor(255, 140, 0));
                    item->setBackground(QColor(255, 235, 200));
                } else {
                    item->setForeground(Qt::darkGreen);
                    item->setBackground(QColor(200, 255, 200));
                }
            }
            
            budgetTable->setItem(row, col, item);
        }
    }
}

void MainWindow::updateTransactionTable()
{
    transactionTable->setRowCount(5);
    
    // 示例数据
    QStringList transactionData = {
        "2024-01-15", "14:30", "支出", "¥150.00", "餐饮", "午餐",
        "2024-01-15", "08:15", "支出", "¥80.00", "交通", "交通卡充值",
        "2024-01-14", "19:20", "支出", "¥300.00", "购物", "超市购物",
        "2024-01-10", "09:00", "收入", "¥5,000.00", "工资", "工资",
        "2024-01-08", "20:45", "支出", "¥200.00", "娱乐", "电影票"
    };
    
    for (int row = 0; row < 5; ++row) {
        for (int col = 0; col < 6; ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(transactionData[row * 6 + col]);
            
            // 设置金额颜色
            if (col == 2) {
                if (transactionData[row * 6 + col] == "支出") {
                    item->setForeground(Qt::red);
                } else {
                    item->setForeground(Qt::darkGreen);
                }
            }
            if (col == 3) {
                if (transactionData[row * 6 + col].startsWith("¥-")) {
                    item->setForeground(Qt::red);
                } else {
                    item->setForeground(Qt::darkGreen);
                }
            }
            
            transactionTable->setItem(row, col, item);
        }
    }
}

void MainWindow::updateAlerts()
{
    alertList->clear();
    
    // 示例预警信息
    alertList->addItem("⚠️ 娱乐预算已超支！当前支出：¥200.00，超出预算：¥0.00");
    alertList->addItem("⚠️ 购物预算使用率已达50%，请注意控制支出");
    
    // 设置预警颜色
    for (int i = 0; i < alertList->count(); ++i) {
        QListWidgetItem *item = alertList->item(i);
        if (item->text().contains("超支")) {
            item->setForeground(Qt::red);
            item->setBackground(QColor(255, 200, 200));
        } else {
            item->setForeground(QColor(255, 140, 0));
            item->setBackground(QColor(255, 245, 200));
        }
    }
}