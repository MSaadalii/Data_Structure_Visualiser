#ifndef OPERATIONPAGE_H
#define OPERATIONPAGE_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QPauseAnimation>
#include <QVector>
#include <QGraphicsDropShadowEffect>

class OperationPage : public QWidget
{
    Q_OBJECT

public:
    explicit OperationPage(const QString &dataStructureName, QWidget *parent = nullptr);
    ~OperationPage();

signals:
    void backToMenu();
    void operationSelected(const QString &operation);

protected:
    void paintEvent(QPaintEvent *event) override;
    void showEvent(QShowEvent *event) override;

private slots:
    void onBackClicked();
    void onInsertionClicked();
    void onDeletionClicked();
    void onSearchClicked();
    void onTraversalClicked();

private:
    void setupUI();
    void setupAnimations();
    QWidget* createOperationCard(const QString &title, const QString &description,
                                 const QString &icon, QPushButton* &button);
    void styleCard(QWidget *card);
    void styleButton(QPushButton *button);
    void styleBackButton(QPushButton *button);

    QString dsName;
    QLabel *titleLabel;
    QLabel *subtitleLabel;
    QPushButton *backButton;
    QPushButton *insertButton;
    QPushButton *deleteButton;
    QPushButton *searchButton;
    QPushButton *traversalButton;

    QWidget *contentContainer;
    QVector<QWidget*> cards;
    QVector<QGraphicsOpacityEffect*> cardOpacities;
    QParallelAnimationGroup *animationGroup;

    bool animationsStarted;
};

#endif // OPERATIONPAGE_H
