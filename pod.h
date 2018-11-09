#ifndef POD_H
#define POD_H

#include <QString>

class Pod {
public:
    Pod();

    Pod(QString project, QString name);

    QString getProject() const;

    void setProject(const QString &value);

    QString getName() const;

    void setName(const QString &value);

private:
    QString project;
    QString name;
};

#endif // POD_H
