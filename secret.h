#ifndef SECRET_H
#define SECRET_H

#include <QString>
#include <QList>
#include <QMap>

class Secret {
public:
    Secret(QString name, QString value, QMap <QString, QString> data);

    QMap <QString, QString> getData() const;

    void setData(const QMap <QString, QString> &value);

    QString getValue() const;

    void setValue(const QString &value);

    QString getName() const;

    void setName(const QString &value);

private:
    QString name;
    QString value;
    QMap <QString, QString> data;
};

#endif // SECRET_H
