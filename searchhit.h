#ifndef SEARCHHIT_H
#define SEARCHHIT_H

#include <QString>
#include <QTextCursor>


class SearchHit {
public:
    SearchHit(QString value, QTextCursor cursor);

    QString getValue() const;

    void setValue(const QString &value);

    QTextCursor getCursor() const;

    void setCursor(const QTextCursor &value);

private:
    QString value;
    QTextCursor cursor;
};

#endif // SEARCHHIT_H
