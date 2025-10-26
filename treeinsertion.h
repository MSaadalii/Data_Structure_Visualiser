#ifndef TREEINSERTION_H
#define TREEINSERTION_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QTimer>
#include <QVector>
#include <QGraphicsOpacityEffect>

// Tree Node structure
struct TreeNode {
    int value;
    TreeNode *left;
    TreeNode *right;
    int x, y;  // Position for drawing
    bool isHighlighted;
    bool isNewNode;

    TreeNode(int val) : value(val), left(nullptr), right(nullptr),
        x(0), y(0), isHighlighted(false), isNewNode(false) {}
};

class TreeInsertion : public QWidget
{
    Q_OBJECT

public:
    explicit TreeInsertion(QWidget *parent = nullptr);
    ~TreeInsertion();

signals:
    void backToOperations();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onBackClicked();
    void onInsertClicked();
    void onClearClicked();

private:
    void setupUI();
    void insertNode(int value);
    void animateInsertion(int value);
    void calculateNodePositions(TreeNode *node, int x, int y, int horizontalSpacing);
    void drawTree(QPainter &painter, TreeNode *node);
    void drawNode(QPainter &painter, TreeNode *node);
    void drawEdge(QPainter &painter, int x1, int y1, int x2, int y2);
    void clearTree(TreeNode *node);
    int getTreeHeight(TreeNode *node);
    void resetHighlights(TreeNode *node);
    void animateTraversal(TreeNode *node, int value, int step);

    // UI Components
    QPushButton *backButton;
    QPushButton *insertButton;
    QPushButton *clearButton;
    QLineEdit *inputField;
    QLabel *titleLabel;
    QLabel *instructionLabel;
    QLabel *statusLabel;
    QWidget *canvasWidget;

    // Tree data
    TreeNode *root;

    // Animation
    QTimer *animationTimer;
    QVector<TreeNode*> traversalPath;
    int currentTraversalStep;
    bool isAnimating;

    // Drawing constants
    const int NODE_RADIUS = 25;
    const int LEVEL_HEIGHT = 80;
    const int CANVAS_TOP_MARGIN = 60;
};

#endif // TREEINSERTION_H
