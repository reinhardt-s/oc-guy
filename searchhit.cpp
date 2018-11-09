#include "searchhit.h"

SearchHit::SearchHit(QString value, QTextCursor cursor) {
    this->cursor = cursor;
    this->value = value;
}

QString SearchHit::getValue() const {
    return value;
}

void SearchHit::setValue(const QString &value) {
    this->value = value;
}

QTextCursor SearchHit::getCursor() const {
    return cursor;
}

void SearchHit::setCursor(const QTextCursor &value) {
    cursor = value;
}
