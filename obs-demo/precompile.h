#pragma once

#include <QMessageBox>
#include <QString>
#include <QTimer>

//#define String(text) QString::fromLocal8Bit(text)
template<typename T>
constexpr auto String(T text) { return QString::fromLocal8Bit(text); }