#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonDocument>
#include <QRegularExpression>
#include <QStringList>
#include <QTextCharFormat>
#include <QFont>
#include <QtGlobal>
#include <QFontDatabase>
#include <QRegExp>
#include <QTextFormat>
#include <QColor>
#include <QObject>
#include <QListIterator>
#include <QWidget>
#include <QSyntaxHighlighter>
#include <QCodeEditor>
#include <QNetworkCookieJar>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QList>
#include <QNetworkCookie>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    ,m_codeEditor(nullptr)
    ,m_codeEditor2(nullptr)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_codeEditor = new QCodeEditor(this);
    m_codeEditor->setObjectName(QString::fromUtf8("codeEditor"));
    m_codeEditor->setGeometry(QRect(40, 10, 450, 250));
    m_codeEditor2 = new QCodeEditor(this);
    m_codeEditor2->setObjectName(QString::fromUtf8("codeEditor2"));
    m_codeEditor2->setGeometry(QRect(510, 10, 450, 250));
    m_codeEditor->hide();
    m_codeEditor2->hide();
    ui->tabWidget->hide();
    ui->textEdit->hide();
    ui->textEdit_2->hide();
    ui->pushButton->hide();


}

MainWindow::~MainWindow()
{
    delete ui;
}

#include <QTextEdit>
void MainWindow::on_pushButton_2_clicked()
{
    ui->pushButton_2->hide();
    QHttpMultiPart * multiPart = new QHttpMultiPart ( QHttpMultiPart :: FormDataType);

    QHttpPart textPartLogin;
    textPartLogin.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"login\""));
    textPartLogin.setBody(ui->textEdit_7->toPlainText().toUtf8());

    QHttpPart textPartPassword;
    textPartPassword.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"password\""));
    textPartPassword.setBody(ui->lineEdit->text().toUtf8());

    QHttpPart textPartAction;
    textPartAction.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"action\""));
    textPartAction.setBody("login");

    multiPart->append(textPartAction);
    multiPart->append(textPartPassword);
    multiPart->append(textPartLogin);

    auto manager = new QNetworkAccessManager();

    QNetworkReply *reply;
    QNetworkRequest request;
    //request.setRawHeader("Content-Type", "multipart/form-data");
    request.setUrl(QUrl("http://vega.fcyb.mirea.ru/auth/action.php"));

    reply = manager->post(request,multiPart);
    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

     ui->pushButton_2->show();

    if (reply->rawHeader("Location")=="../login.php" ){
        QMessageBox::critical(this,"Error","Invalid login or password");
        ui->textEdit_7->clear();
        ui->lineEdit->clear();
    }
    else {
        ui->label->hide();
        ui->label_2->hide();
        ui->textEdit_7->hide();
        ui->lineEdit->hide();
        ui->pushButton_2->hide();
        m_codeEditor->show();
        m_codeEditor2->show();
        ui->tabWidget->show();
        ui->textEdit->show();
        ui->textEdit_2->show();
        ui->pushButton->show();
    }
    if (reply->hasRawHeader("Set-Cookie")){
        cookieHeader =  new QByteArray(reply->rawHeader("Set-Cookie"));
    }
}
void MainWindow::on_pushButton_clicked()
{
    ui->pushButton->hide();
    auto manager = new QNetworkAccessManager();

    QJsonObject postObject
    {
           {"spec",m_codeEditor2->toPlainText()},
           {"example",m_codeEditor->toPlainText()}
    };

    QNetworkReply *reply;
    QNetworkRequest request;
    request.setRawHeader("Content-Type", "application/json");

    cookieHeader->replace("; path=/","");
    request.setRawHeader("Cookie",*cookieHeader);

    request.setUrl(QUrl("http://vega.fcyb.mirea.ru/testcpp/api/v1/specRunner"));

    QByteArray postData;
    QJsonDocument postDocument;
    postDocument.setObject(postObject);
    postData.append(postDocument.toJson());

    reply = manager->post(request,postData);
    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    QByteArray wholeArrayReply = reply->readAll();
    QString wholeStringReply(wholeArrayReply);
    QJsonDocument wholeJsonReply = QJsonDocument::fromJson(wholeArrayReply);
    QJsonObject wholeJsonOblectReply = wholeJsonReply.object();

    ui->pushButton->show();

    if (wholeJsonOblectReply["specCompilerResult"]!=0){
    QJsonObject specCompilerResult = wholeJsonOblectReply.value(wholeJsonOblectReply.keys().at(0)).toObject();
    QString specCompilerResultInString(specCompilerResult.value(specCompilerResult.keys().at(0)).toString()+specCompilerResult.value(specCompilerResult.keys().at(1)).toString()+specCompilerResult.value(specCompilerResult.keys().at(2)).toString());
    specCompilerResultInString.replace(QString("\\033[K"),QString("\n"));
    specCompilerResultInString.replace(QString("\\u001[K"),QString("\n"));
    specCompilerResultInString.replace(QString("[K"),QString("\n"));
    specCompilerResultInString.remove(QString("\\033[m"));
    specCompilerResultInString.remove(QString("\\u001[m"));
    specCompilerResultInString.remove(QString("[m"));
    specCompilerResultInString.remove(QString("\\r"));
    specCompilerResultInString.replace(QString("\\n"),QString("\n"));
    if (specCompilerResult.value(specCompilerResult.keys().at(0)).toInt()!=1){
        ui->textEdit_4->clear();
        setTextTermFormatting(ui->textEdit_4,specCompilerResultInString);
    }
    else{
        ui->textEdit_3->clear();
        setTextTermFormatting(ui->textEdit_3,specCompilerResultInString);
    }

    if (specCompilerResult.value(specCompilerResult.keys().at(0)).toInt()!=1){
        QJsonObject exampleCompilerResult = wholeJsonOblectReply.value(wholeJsonOblectReply.keys().at(1)).toObject();
        QString exampleCompilerResultInString=exampleCompilerResult.value(exampleCompilerResult.keys().at(0)).toString()+exampleCompilerResult.value(exampleCompilerResult.keys().at(1)).toString()+exampleCompilerResult.value(exampleCompilerResult.keys().at(2)).toString();
        exampleCompilerResultInString.replace(QString("\\033[K"),QString("\n"));
        exampleCompilerResultInString.replace(QString("\\u001[K"),QString("\n"));
        exampleCompilerResultInString.replace(QString("[K"),QString("\n"));
        exampleCompilerResultInString.remove(QString("\\033[m"));
        exampleCompilerResultInString.remove(QString("\\u001[m"));
        exampleCompilerResultInString.remove(QString("[m"));
        exampleCompilerResultInString.remove(QString("\\r"));
        exampleCompilerResultInString.replace(QString("\\n"),QString("\n"));
        ui->textEdit_5->clear();
        setTextTermFormatting(ui->textEdit_5,exampleCompilerResultInString);

        if(exampleCompilerResult.value(specCompilerResult.keys().at(0)).toInt()!=1){
            QJsonObject linkerResult = wholeJsonOblectReply.value(wholeJsonOblectReply.keys().at(2)).toObject();
            QString linkerResultInString(linkerResult.value(linkerResult.keys().at(0)).toString()+linkerResult.value(linkerResult.keys().at(1)).toString()+linkerResult.value(linkerResult.keys().at(2)).toString());
            linkerResultInString.replace(QString("\\033[K"),QString("\n"));
            linkerResultInString.replace(QString("\\u001[K"),QString("\n"));
            linkerResultInString.replace(QString("[K"),QString("\n"));
            linkerResultInString.remove(QString("\\033[m"));
            linkerResultInString.remove(QString("\\u001[m"));
            linkerResultInString.remove(QString("[m"));
            linkerResultInString.remove(QString("\\r"));
            linkerResultInString.replace(QString("\\n"),QString("\n"));
            ui->textEdit_6->clear();
            setTextTermFormatting(ui->textEdit_6,linkerResultInString);

            if(linkerResult.value(specCompilerResult.keys().at(0)).toInt()!=1){
                QJsonObject runnerResult = wholeJsonOblectReply.value(wholeJsonOblectReply.keys().at(3)).toObject();
                QString runnerResultInString(runnerResult.value(runnerResult.keys().at(0)).toString()+runnerResult.value(runnerResult.keys().at(1)).toString()+runnerResult.value(runnerResult.keys().at(2)).toString());
                runnerResultInString.replace(QString("\\033[K"),QString("\n"));
                runnerResultInString.replace(QString("\\u001[K"),QString("\n"));
                runnerResultInString.replace(QString("[K"),QString("\n"));
                runnerResultInString.remove(QString("\\033[m"));
                runnerResultInString.remove(QString("\\u001[m"));
                runnerResultInString.remove(QString("[m"));
                runnerResultInString.remove(QString("\\r"));
                runnerResultInString.replace(QString("\\n"),QString("\n"));
                ui->textEdit_3->clear();
                setTextTermFormatting(ui->textEdit_3,runnerResultInString);
            }
        }
    }
    }

}

void MainWindow::parseEscapeSequence(int attribute, QListIterator< QString > & i, QTextCharFormat & textCharFormat, QTextCharFormat const & defaultTextCharFormat)
{
    switch (attribute) {
    case 0 : { // Normal/Default (reset all attributes)
        textCharFormat = defaultTextCharFormat;
        break;
    }
    case 1 : { // Bold/Bright (bold or increased intensity)
        textCharFormat.setFontWeight(QFont::Bold);
        break;
    }
    case 2 : { // Dim/Faint (decreased intensity)
        textCharFormat.setFontWeight(QFont::Light);
        break;
    }
    case 3 : { // Italicized (italic on)
        textCharFormat.setFontItalic(true);
        break;
    }
    case 4 : { // Underscore (single underlined)
        textCharFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);
        textCharFormat.setFontUnderline(true);
        break;
    }
    case 5 : { // Blink (slow, appears as Bold)
        textCharFormat.setFontWeight(QFont::Bold);
        break;
    }
    case 6 : { // Blink (rapid, appears as very Bold)
        textCharFormat.setFontWeight(QFont::Black);
        break;
    }
    case 7 : { // Reverse/Inverse (swap foreground and background)
        QBrush foregroundBrush = textCharFormat.foreground();
        textCharFormat.setForeground(textCharFormat.background());
        textCharFormat.setBackground(foregroundBrush);
        break;
    }
    case 8 : { // Concealed/Hidden/Invisible (usefull for passwords)
        textCharFormat.setForeground(textCharFormat.background());
        break;
    }
    case 9 : { // Crossed-out characters
        textCharFormat.setFontStrikeOut(true);
        break;
    }
    case 10 : { // Primary (default) font
        textCharFormat.setFont(defaultTextCharFormat.font());
        break;
    }
    case 11 ... 19 : {
        QFontDatabase fontDatabase;
        QString fontFamily = textCharFormat.fontFamily();
        QStringList fontStyles = fontDatabase.styles(fontFamily);
        int fontStyleIndex = attribute - 11;
        if (fontStyleIndex < fontStyles.length()) {
            textCharFormat.setFont(fontDatabase.font(fontFamily, fontStyles.at(fontStyleIndex), textCharFormat.font().pointSize()));
        }
        break;
    }
    case 20 : { // Fraktur (unsupported)
        break;
    }
    case 21 : { // Set Bold off
        textCharFormat.setFontWeight(QFont::Normal);
        break;
    }
    case 22 : { // Set Dim off
        textCharFormat.setFontWeight(QFont::Normal);
        break;
    }
    case 23 : { // Unset italic and unset fraktur
        textCharFormat.setFontItalic(false);
        break;
    }
    case 24 : { // Unset underlining
        textCharFormat.setUnderlineStyle(QTextCharFormat::NoUnderline);
        textCharFormat.setFontUnderline(false);
        break;
    }
    case 25 : { // Unset Blink/Bold
        textCharFormat.setFontWeight(QFont::Normal);
        break;
    }
    case 26 : { // Reserved
        break;
    }
    case 27 : { // Positive (non-inverted)
        QBrush backgroundBrush = textCharFormat.background();
        textCharFormat.setBackground(textCharFormat.foreground());
        textCharFormat.setForeground(backgroundBrush);
        break;
    }
    case 28 : {
        textCharFormat.setForeground(defaultTextCharFormat.foreground());
        textCharFormat.setBackground(defaultTextCharFormat.background());
        break;
    }
    case 29 : {
        textCharFormat.setUnderlineStyle(QTextCharFormat::NoUnderline);
        textCharFormat.setFontUnderline(false);
        break;
    }
    case 30 ... 37 : {
        int colorIndex = attribute - 30;
        QColor color;
        if (QFont::Normal < textCharFormat.fontWeight()) {
            switch (colorIndex) {
            case 0 : {
                color = Qt::darkGray;
                break;
            }
            case 1 : {
                color = Qt::red;
                break;
            }
            case 2 : {
                color = Qt::green;
                break;
            }
            case 3 : {
                color = Qt::yellow;
                break;
            }
            case 4 : {
                color = Qt::blue;
                break;
            }
            case 5 : {
                color = Qt::magenta;
                break;
            }
            case 6 : {
                color = Qt::cyan;
                break;
            }
            case 7 : {
                color = Qt::white;
                break;
            }
            default : {
                Q_ASSERT(false);
            }
            }
        } else {
            switch (colorIndex) {
            case 0 : {
                color = Qt::black;
                break;
            }
            case 1 : {
                color = Qt::darkRed;
                break;
            }
            case 2 : {
                color = Qt::darkGreen;
                break;
            }
            case 3 : {
                color = Qt::darkYellow;
                break;
            }
            case 4 : {
                color = Qt::darkBlue;
                break;
            }
            case 5 : {
                color = Qt::darkMagenta;
                break;
            }
            case 6 : {
                color = Qt::darkCyan;
                break;
            }
            case 7 : {
                color = Qt::lightGray;
                break;
            }
            default : {
                Q_ASSERT(false);
            }
            }
        }
        textCharFormat.setForeground(color);
        break;
    }
    case 38 : {
        if (i.hasNext()) {
            bool ok = false;
            int selector = i.next().toInt(&ok);
            Q_ASSERT(ok);
            QColor color;
            switch (selector) {
            case 2 : {
                if (!i.hasNext()) {
                    break;
                }
                int red = i.next().toInt(&ok);
                Q_ASSERT(ok);
                if (!i.hasNext()) {
                    break;
                }
                int green = i.next().toInt(&ok);
                Q_ASSERT(ok);
                if (!i.hasNext()) {
                    break;
                }
                int blue = i.next().toInt(&ok);
                Q_ASSERT(ok);
                color.setRgb(red, green, blue);
                break;
            }
            case 5 : {
                if (!i.hasNext()) {
                    break;
                }
                int index = i.next().toInt(&ok);
                Q_ASSERT(ok);
                switch (index) {
                case 0x00 ... 0x07 : { // 0x00-0x07:  standard colors (as in ESC [ 30..37 m)
                    return parseEscapeSequence(index - 0x00 + 30, i, textCharFormat, defaultTextCharFormat);
                }
                case 0x08 ... 0x0F : { // 0x08-0x0F:  high intensity colors (as in ESC [ 90..97 m)
                    return parseEscapeSequence(index - 0x08 + 90, i, textCharFormat, defaultTextCharFormat);
                }
                case 0x10 ... 0xE7 : { // 0x10-0xE7:  6*6*6=216 colors: 16 + 36*r + 6*g + b (0≤r,g,b≤5)
                    index -= 0x10;
                    int red = index % 6;
                    index /= 6;
                    int green = index % 6;
                    index /= 6;
                    int blue = index % 6;
                    index /= 6;
                    Q_ASSERT(index == 0);
                    color.setRgb(red, green, blue);
                    break;
                }
                case 0xE8 ... 0xFF : { // 0xE8-0xFF:  grayscale from black to white in 24 steps
                    qreal intensity = qreal(index - 0xE8) / (0xFF - 0xE8);
                    color.setRgbF(intensity, intensity, intensity);
                    break;
                }
                }
                textCharFormat.setForeground(color);
                break;
            }
            default : {
                break;
            }
            }
        }
        break;
    }
    case 39 : {
        textCharFormat.setForeground(defaultTextCharFormat.foreground());
        break;
    }
    case 40 ... 47 : {
        int colorIndex = attribute - 40;
        QColor color;
        switch (colorIndex) {
        case 0 : {
            color = Qt::darkGray;
            break;
        }
        case 1 : {
            color = Qt::red;
            break;
        }
        case 2 : {
            color = Qt::green;
            break;
        }
        case 3 : {
            color = Qt::yellow;
            break;
        }
        case 4 : {
            color = Qt::blue;
            break;
        }
        case 5 : {
            color = Qt::magenta;
            break;
        }
        case 6 : {
            color = Qt::cyan;
            break;
        }
        case 7 : {
            color = Qt::white;
            break;
        }
        default : {
            Q_ASSERT(false);
        }
        }
        textCharFormat.setBackground(color);
        break;
    }
    case 48 : {
        if (i.hasNext()) {
            bool ok = false;
            int selector = i.next().toInt(&ok);
            Q_ASSERT(ok);
            QColor color;
            switch (selector) {
            case 2 : {
                if (!i.hasNext()) {
                    break;
                }
                int red = i.next().toInt(&ok);
                Q_ASSERT(ok);
                if (!i.hasNext()) {
                    break;
                }
                int green = i.next().toInt(&ok);
                Q_ASSERT(ok);
                if (!i.hasNext()) {
                    break;
                }
                int blue = i.next().toInt(&ok);
                Q_ASSERT(ok);
                color.setRgb(red, green, blue);
                break;
            }
            case 5 : {
                if (!i.hasNext()) {
                    break;
                }
                int index = i.next().toInt(&ok);
                Q_ASSERT(ok);
                switch (index) {
                case 0x00 ... 0x07 : { // 0x00-0x07:  standard colors (as in ESC [ 40..47 m)
                    return parseEscapeSequence(index - 0x00 + 40, i, textCharFormat, defaultTextCharFormat);
                }
                case 0x08 ... 0x0F : { // 0x08-0x0F:  high intensity colors (as in ESC [ 100..107 m)
                    return parseEscapeSequence(index - 0x08 + 100, i, textCharFormat, defaultTextCharFormat);
                }
                case 0x10 ... 0xE7 : { // 0x10-0xE7:  6*6*6=216 colors: 16 + 36*r + 6*g + b (0≤r,g,b≤5)
                    index -= 0x10;
                    int red = index % 6;
                    index /= 6;
                    int green = index % 6;
                    index /= 6;
                    int blue = index % 6;
                    index /= 6;
                    Q_ASSERT(index == 0);
                    color.setRgb(red, green, blue);
                    break;
                }
                case 0xE8 ... 0xFF : { // 0xE8-0xFF:  grayscale from black to white in 24 steps
                    qreal intensity = qreal(index - 0xE8) / (0xFF - 0xE8);
                    color.setRgbF(intensity, intensity, intensity);
                    break;
                }
                }
                textCharFormat.setBackground(color);
                break;
            }
            default : {
                break;
            }
            }
        }
        break;
    }
    case 49 : {
        textCharFormat.setBackground(defaultTextCharFormat.background());
        break;
    }
    case 90 ... 97 : {
        int colorIndex = attribute - 90;
        QColor color;
        switch (colorIndex) {
        case 0 : {
            color = Qt::darkGray;
            break;
        }
        case 1 : {
            color = Qt::red;
            break;
        }
        case 2 : {
            color = Qt::green;
            break;
        }
        case 3 : {
            color = Qt::yellow;
            break;
        }
        case 4 : {
            color = Qt::blue;
            break;
        }
        case 5 : {
            color = Qt::magenta;
            break;
        }
        case 6 : {
            color = Qt::cyan;
            break;
        }
        case 7 : {
            color = Qt::white;
            break;
        }
        default : {
            Q_ASSERT(false);
        }
        }
        color.setRedF(color.redF() * 0.8);
        color.setGreenF(color.greenF() * 0.8);
        color.setBlueF(color.blueF() * 0.8);
        textCharFormat.setForeground(color);
        break;
    }
    case 100 ... 107 : {
        int colorIndex = attribute - 100;
        QColor color;
        switch (colorIndex) {
        case 0 : {
            color = Qt::darkGray;
            break;
        }
        case 1 : {
            color = Qt::red;
            break;
        }
        case 2 : {
            color = Qt::green;
            break;
        }
        case 3 : {
            color = Qt::yellow;
            break;
        }
        case 4 : {
            color = Qt::blue;
            break;
        }
        case 5 : {
            color = Qt::magenta;
            break;
        }
        case 6 : {
            color = Qt::cyan;
            break;
        }
        case 7 : {
            color = Qt::white;
            break;
        }
        default : {
            Q_ASSERT(false);
        }
        }
        color.setRedF(color.redF() * 0.8);
        color.setGreenF(color.greenF() * 0.8);
        color.setBlueF(color.blueF() * 0.8);
        textCharFormat.setBackground(color);
        break;
    }
    default : {
        break;
    }
    }
}

void MainWindow::setTextTermFormatting(QTextEdit * textEdit, QString const & text)
{
    QTextDocument * document = textEdit->document();
    QRegExp const escapeSequenceExpression(R"((?:|\\u001)\[([\d;]+)m)");
    QTextCursor cursor(document);
    QTextCharFormat const defaultTextCharFormat = cursor.charFormat();
    cursor.beginEditBlock();
    int offset = escapeSequenceExpression.indexIn(text);
    cursor.insertText(text.mid(0, offset));
    QTextCharFormat textCharFormat = defaultTextCharFormat;
    while (!(offset < 0)) {
        ui->textEdit_2->setText("offset");
        int previousOffset = offset + escapeSequenceExpression.matchedLength();
        QStringList capturedTexts = escapeSequenceExpression.capturedTexts().back().split(';');
        QListIterator< QString > i(capturedTexts);
        while (i.hasNext()) {
            bool ok = false;
            int attribute = i.next().toInt(&ok);
            Q_ASSERT(ok);
            parseEscapeSequence(attribute, i, textCharFormat, defaultTextCharFormat);
        }
        offset = escapeSequenceExpression.indexIn(text, previousOffset);
        if (offset < 0) {
            cursor.insertText(text.mid(previousOffset), textCharFormat);
        } else {
            cursor.insertText(text.mid(previousOffset, offset - previousOffset), textCharFormat);
        }
    }
    cursor.setCharFormat(defaultTextCharFormat);
    cursor.endEditBlock();
    //cursor.movePosition(QTextCursor::Start);
    textEdit->setTextCursor(cursor);
}

