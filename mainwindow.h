#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextCharFormat>
#include <QTextEdit>
#include <QCodeEditor>
#include <QNetworkAccessManager>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class QCodeEditor;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



private slots:
    void on_pushButton_clicked();
    void parseEscapeSequence(int attribute, QListIterator< QString > & i, QTextCharFormat & textCharFormat, QTextCharFormat const & defaultTextCharFormat);
    void setTextTermFormatting(QTextEdit * textEdit, QString const & text);

    void on_pushButton_2_clicked();

private:
    QCodeEditor* m_codeEditor;
    QCodeEditor* m_codeEditor2;
    QNetworkCookieJar* cook;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
