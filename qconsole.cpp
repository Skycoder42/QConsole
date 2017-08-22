#include "qconsole.h"
#include "qconsolethread_p.h"
#include <QDebug>
#include <iostream>
#include <string>

QConsole::QConsole(QObject *parent) :
	QIODevice(parent),
	_consoleThread(new QConsoleThread(this)),
	_notifier(new QSocketNotifier(fileno(stdin), QSocketNotifier::Read, this)),
	_writeMode(WriteStdOut)
{
	connect(_notifier, &QSocketNotifier::activated, this, [this](int s){
		qDebug() << "Data available on" << s;
		emit readyRead();
	});
}

QConsole::~QConsole()
{
	if(isOpen())
		close();
}

bool QConsole::isSequential() const
{
	return true;
}

bool QConsole::open(OpenMode mode)
{
	if(QIODevice::open(mode | QIODevice::Unbuffered)) {
		//_consoleThread->start();
		_notifier->setEnabled(true);
		return true;
	} else
		return false;
}

void QConsole::close()
{
	if(isOpen()) {
		_notifier->setEnabled(false);
//		_consoleThread->requestInterruption();
//		if(!_consoleThread->wait(5000)) {
//			_consoleThread->terminate();
//			_consoleThread->wait(500);
//		}
	}
	QIODevice::close();
}

qint64 QConsole::bytesAvailable() const
{
	return 0;
}

QConsole::WriteMode QConsole::writeMode() const
{
	return _writeMode;
}

void QConsole::setWriteMode(WriteMode writeMode)
{
	_writeMode = writeMode;
}

void QConsole::flush()
{
	Q_UNIMPLEMENTED();
}

qint64 QConsole::readData(char *data, qint64 maxlen)
{
	return 0;
}

qint64 QConsole::writeData(const char *data, qint64 len)
{
	return len;
}
