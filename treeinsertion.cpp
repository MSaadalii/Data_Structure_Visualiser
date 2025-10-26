#include "treeinsertion.h"
#include <QPainter>
#include <QLinearGradient>
#include <QFont>
#include <QFontDatabase>
#include <QMessageBox>
#include <QDebug>
#include <cmath>

TreeInsertion::TreeInsertion(QWidget *parent)
    : QWidget(parent)
    , root(nullptr)
    , currentTraversalStep(0)
    , isAnimating(false)
{
    setupUI();
    setMinimumSize(900, 750);

    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, [this]() {
        update();
    });
}

TreeInsertion::~TreeInsertion()
{
    clearTree(root);
}

void TreeInsertion::setupUI()
{
    // Main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);

    // Top section - Header and controls
    QVBoxLayout *topSection = new QVBoxLayout();
    topSection->setSpacing(15);

    // Back button
    backButton = new QPushButton("← Back to Operations", this);
    backButton->setFixedSize(160, 38);
    backButton->setCursor(Qt::PointingHandCursor);

    QFont buttonFont("Segoe UI", 11);
    backButton->setFont(buttonFont);
    backButton->setStyleSheet(R"(
        QPushButton {
            background-color: rgba(123, 79, 255, 0.1);
            color: #7b4fff;
            border: 2px solid #7b4fff;
            border-radius: 19px;
            padding: 8px 16px;
        }
        QPushButton:hover {
            background-color: rgba(123, 79, 255, 0.2);
        }
    )");

    topSection->addWidget(backButton, 0, Qt::AlignLeft);

    // Title
    titleLabel = new QLabel("Binary Tree - Insertion", this);
    QFont titleFont("Segoe UI", 28, QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #2d1b69;");
    titleLabel->setAlignment(Qt::AlignCenter);
    topSection->addWidget(titleLabel);

    // Instruction label
    instructionLabel = new QLabel("Enter a value to insert into the tree", this);
    QFont instructionFont("Segoe UI", 12);
    instructionLabel->setFont(instructionFont);
    instructionLabel->setStyleSheet("color: #6b5b95;");
    instructionLabel->setAlignment(Qt::AlignCenter);
    topSection->addWidget(instructionLabel);

    mainLayout->addLayout(topSection);

    // Control panel
    QHBoxLayout *controlLayout = new QHBoxLayout();
    controlLayout->setSpacing(15);
    controlLayout->addStretch();

    // Input field
    inputField = new QLineEdit(this);
    inputField->setPlaceholderText("Enter value (e.g., 42)");
    inputField->setFixedSize(200, 45);
    inputField->setAlignment(Qt::AlignCenter);
    QFont inputFont("Segoe UI", 13);
    inputField->setFont(inputFont);
    inputField->setStyleSheet(R"(
        QLineEdit {
            background-color: white;
            border: 2px solid #d0c5e8;
            border-radius: 22px;
            padding: 8px 16px;
            color: #2d1b69;
        }
        QLineEdit:focus {
            border-color: #7b4fff;
        }
    )");

    // Insert button
    insertButton = new QPushButton("Insert", this);
    insertButton->setFixedSize(120, 45);
    insertButton->setCursor(Qt::PointingHandCursor);
    QFont insertFont("Segoe UI", 13, QFont::Bold);
    insertButton->setFont(insertFont);
    insertButton->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #7b4fff, stop:1 #9b6fff);
            color: white;
            border: none;
            border-radius: 22px;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #6c3cff, stop:1 #8b5fff);
        }
        QPushButton:pressed {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #5a32cc, stop:1 #7a4ccc);
        }
        QPushButton:disabled {
            background: #cccccc;
        }
    )");

    // Clear button
    clearButton = new QPushButton("Clear Tree", this);
    clearButton->setFixedSize(120, 45);
    clearButton->setCursor(Qt::PointingHandCursor);
    clearButton->setFont(insertFont);
    clearButton->setStyleSheet(R"(
        QPushButton {
            background-color: rgba(255, 79, 79, 0.9);
            color: white;
            border: none;
            border-radius: 22px;
        }
        QPushButton:hover {
            background-color: rgba(255, 60, 60, 1);
        }
        QPushButton:pressed {
            background-color: rgba(200, 50, 50, 1);
        }
    )");

    controlLayout->addWidget(inputField);
    controlLayout->addWidget(insertButton);
    controlLayout->addWidget(clearButton);
    controlLayout->addStretch();

    mainLayout->addLayout(controlLayout);

    // Status label
    statusLabel = new QLabel("Tree is empty. Start by inserting a value!", this);
    QFont statusFont("Segoe UI", 11);
    statusLabel->setFont(statusFont);
    statusLabel->setStyleSheet("color: #7b4fff; padding: 8px;");
    statusLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(statusLabel);

    // Canvas for tree visualization - this is now just for layout, we draw directly on widget
    mainLayout->addStretch();

    setLayout(mainLayout);

    // Connect signals
    connect(backButton, &QPushButton::clicked, this, &TreeInsertion::onBackClicked);
    connect(insertButton, &QPushButton::clicked, this, &TreeInsertion::onInsertClicked);
    connect(clearButton, &QPushButton::clicked, this, &TreeInsertion::onClearClicked);
    connect(inputField, &QLineEdit::returnPressed, this, &TreeInsertion::onInsertClicked);
}

void TreeInsertion::onBackClicked()
{
    emit backToOperations();
}

void TreeInsertion::onInsertClicked()
{
    if (isAnimating) {
        QMessageBox::warning(this, "Animation in Progress",
                             "Please wait for the current animation to complete.");
        return;
    }

    QString text = inputField->text().trimmed();
    if (text.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "Please enter a value.");
        return;
    }

    bool ok;
    int value = text.toInt(&ok);

    if (!ok) {
        QMessageBox::warning(this, "Invalid Input", "Please enter a valid integer.");
        return;
    }

    animateInsertion(value);
    inputField->clear();
    inputField->setFocus();
}

void TreeInsertion::onClearClicked()
{
    if (isAnimating) {
        QMessageBox::warning(this, "Animation in Progress",
                             "Please wait for the current animation to complete.");
        return;
    }

    clearTree(root);
    root = nullptr;
    statusLabel->setText("Tree cleared! Start by inserting a value.");
    update();
}

void TreeInsertion::insertNode(int value)
{
    TreeNode *newNode = new TreeNode(value);
    newNode->isNewNode = true;

    if (!root) {
        root = newNode;
        statusLabel->setText(QString("Inserted %1 as root node").arg(value));
        return;
    }

    TreeNode *current = root;
    TreeNode *parent = nullptr;

    while (current) {
        parent = current;
        if (value < current->value) {
            current = current->left;
        } else if (value > current->value) {
            current = current->right;
        } else {
            delete newNode;
            statusLabel->setText(QString("Value %1 already exists in tree!").arg(value));
            return;
        }
    }

    if (value < parent->value) {
        parent->left = newNode;
        statusLabel->setText(QString("Inserted %1 as left child of %2").arg(value).arg(parent->value));
    } else {
        parent->right = newNode;
        statusLabel->setText(QString("Inserted %1 as right child of %2").arg(value).arg(parent->value));
    }
}

void TreeInsertion::animateInsertion(int value)
{
    isAnimating = true;
    insertButton->setEnabled(false);
    clearButton->setEnabled(false);

    // Build traversal path
    traversalPath.clear();

    if (!root) {
        insertNode(value);
        isAnimating = false;
        insertButton->setEnabled(true);
        clearButton->setEnabled(true);
        update();
        return;
    }

    TreeNode *current = root;
    while (current) {
        traversalPath.append(current);
        if (value < current->value) {
            if (!current->left) {
                break;
            }
            current = current->left;
        } else if (value > current->value) {
            if (!current->right) {
                break;
            }
            current = current->right;
        } else {
            // Duplicate value
            QMessageBox::warning(this, "Duplicate Value",
                                 QString("Value %1 already exists in tree!").arg(value));
            isAnimating = false;
            insertButton->setEnabled(true);
            clearButton->setEnabled(true);
            return;
        }
    }

    currentTraversalStep = 0;

    QTimer::singleShot(0, this, [this, value]() {
        animateTraversal(root, value, 0);
    });
}

void TreeInsertion::animateTraversal(TreeNode *node, int value, int step)
{
    if (step >= traversalPath.size()) {
        // Animation complete, insert the node
        insertNode(value);
        resetHighlights(root);
        isAnimating = false;
        insertButton->setEnabled(true);
        clearButton->setEnabled(true);
        update();
        return;
    }

    resetHighlights(root);
    traversalPath[step]->isHighlighted = true;

    if (step == 0) {
        statusLabel->setText(QString("Comparing %1 with root %2").arg(value).arg(traversalPath[step]->value));
    } else {
        statusLabel->setText(QString("Comparing %1 with %2").arg(value).arg(traversalPath[step]->value));
    }

    update();

    QTimer::singleShot(800, this, [this, node, value, step]() {
        animateTraversal(node, value, step + 1);
    });
}

void TreeInsertion::calculateNodePositions(TreeNode *node, int x, int y, int horizontalSpacing)
{
    if (!node) return;

    node->x = x;
    node->y = y;

    int nextSpacing = horizontalSpacing / 2;

    if (node->left) {
        calculateNodePositions(node->left, x - horizontalSpacing, y + LEVEL_HEIGHT, nextSpacing);
    }
    if (node->right) {
        calculateNodePositions(node->right, x + horizontalSpacing, y + LEVEL_HEIGHT, nextSpacing);
    }
}

void TreeInsertion::resetHighlights(TreeNode *node)
{
    if (!node) return;
    node->isHighlighted = false;
    node->isNewNode = false;
    resetHighlights(node->left);
    resetHighlights(node->right);
}

void TreeInsertion::clearTree(TreeNode *node)
{
    if (!node) return;
    clearTree(node->left);
    clearTree(node->right);
    delete node;
}

int TreeInsertion::getTreeHeight(TreeNode *node)
{
    if (!node) return 0;
    return 1 + std::max(getTreeHeight(node->left), getTreeHeight(node->right));
}

void TreeInsertion::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Background gradient
    QLinearGradient gradient(0, 0, width(), height());
    gradient.setColorAt(0.0, QColor(250, 247, 255));
    gradient.setColorAt(1.0, QColor(237, 228, 255));
    painter.fillRect(rect(), gradient);

    // Draw white canvas area for tree
    int canvasY = 320; // Position below controls
    int canvasHeight = height() - canvasY - 30;
    QRect canvasRect(30, canvasY, width() - 60, canvasHeight);

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.drawRoundedRect(canvasRect, 16, 16);

    // Draw tree if exists - position relative to canvas
    if (root) {
        int canvasWidth = canvasRect.width();
        int canvasCenterX = canvasRect.x() + canvasWidth / 2;
        int treeStartY = canvasRect.y() + 50;

        calculateNodePositions(root, canvasCenterX, treeStartY, canvasWidth / 4);
        drawTree(painter, root);
    }
}

void TreeInsertion::drawTree(QPainter &painter, TreeNode *node)
{
    if (!node) return;

    // Draw edges first
    if (node->left) {
        drawEdge(painter, node->x, node->y, node->left->x, node->left->y);
        drawTree(painter, node->left);
    }
    if (node->right) {
        drawEdge(painter, node->x, node->y, node->right->x, node->right->y);
        drawTree(painter, node->right);
    }

    // Draw node on top
    drawNode(painter, node);
}

void TreeInsertion::drawNode(QPainter &painter, TreeNode *node)
{
    if (!node) return;

    // Node circle
    if (node->isHighlighted) {
        painter.setPen(QPen(QColor(255, 165, 0), 4));
        painter.setBrush(QColor(255, 200, 100));
    } else if (node->isNewNode) {
        painter.setPen(QPen(QColor(50, 205, 50), 4));
        painter.setBrush(QColor(144, 238, 144));
    } else {
        painter.setPen(QPen(QColor(123, 79, 255), 3));
        painter.setBrush(QColor(200, 180, 255));
    }

    painter.drawEllipse(QPoint(node->x, node->y), NODE_RADIUS, NODE_RADIUS);

    // Node value
    painter.setPen(Qt::black);
    QFont font("Segoe UI", 14, QFont::Bold);
    painter.setFont(font);
    painter.drawText(QRect(node->x - NODE_RADIUS, node->y - NODE_RADIUS,
                           NODE_RADIUS * 2, NODE_RADIUS * 2),
                     Qt::AlignCenter, QString::number(node->value));
}

void TreeInsertion::drawEdge(QPainter &painter, int x1, int y1, int x2, int y2)
{
    painter.setPen(QPen(QColor(123, 79, 255), 2));
    painter.drawLine(x1, y1 + NODE_RADIUS, x2, y2 - NODE_RADIUS);
}
