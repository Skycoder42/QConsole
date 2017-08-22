#include "qconsole.h"
#include <QBuffer>
#include <QFile>
#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif

QConsole::QConsole(QObject *parent) :
	QIODevice(parent),
#ifdef Q_OS_WIN
	_notifier(new QWinEventNotifier(GetStdHandle(STD_INPUT_HANDLE), this)),
#else
	_notifier(new QSocketNotifier(fileno(stdin), QSocketNotifier::Read, this)),
#endif
	_in(new QFile(this)),
	_out(new QFile(this)),
	_err(new QFile(this)),
	_writeMode(WriteStdOut)
{
	_in->setObjectName(QStringLiteral("stdin"));
	_out->setObjectName(QStringLiteral("stdout"));
	_err->setObjectName(QStringLiteral("stderr"));
	_notifier->setEnabled(false);
#ifdef Q_OS_WIN
	connect(_notifier, &QWinEventNotifier::activated,
			this, &QConsole::activated);
#else
	connect(_notifier, &QSocketNotifier::activated,
			this, &QConsole::activated);
#endif
}

QConsole::~QConsole()
{
	if(isOpen())
		close();
#ifdef Q_OS_WIN
	CloseHandle(_notifier->handle());
#endif
}

bool QConsole::isSequential() const
{
	return true;
}

bool QConsole::open(QIODevice::OpenMode openMode)
{
	return open(openMode, WriteStdOut);
}

bool QConsole::open(OpenMode openMode, WriteMode writeMode)
{
	if(QIODevice::open(openMode)) {
		try {
			if(openMode.testFlag(ReadOnly)) {
				if(!_in->open(stdin, ReadOnly | Unbuffered))
					throw _in->errorString();
				_notifier->setEnabled(true);
#ifdef Q_OS_WIN
				FlushConsoleInputBuffer(_notifier->handle());
#endif
			}

			if(openMode.testFlag(WriteOnly)) {
				if(writeMode.testFlag(WriteStdOut)) {
					if(!_out->open(stdout, WriteOnly | Unbuffered))
						throw _out->errorString();
				}
				if(writeMode.testFlag(WriteStdErr)) {
					if(!_err->open(stdout, WriteOnly | Unbuffered))
						throw _err->errorString();
				}
				_writeMode = writeMode;
			} else
				_writeMode = WriteNone;
			return true;
		} catch(const QString &error) {
			close();
			setErrorString(error);
			if(_in->isOpen())
				_in->close();
			if(_out->isOpen())
				_out->close();
			if(_err->isOpen())
				_err->close();
			return false;
		}
	} else
		return false;
}

void QConsole::close()
{
	if(isOpen()) {
		_notifier->setEnabled(false);
		if(_in->isOpen())
			_in->close();
		if(_out->isOpen())
			_out->close();
		if(_err->isOpen())
			_err->close();
	}
	QIODevice::close();
}

qint64 QConsole::bytesAvailable() const
{
	return QIODevice::bytesAvailable() + _in->bytesAvailable();
}

QConsole::WriteMode QConsole::writeMode() const
{
	return _writeMode;
}

void QConsole::flush()
{
	if(_out->isOpen())
		_out->flush();
	if(_err->isOpen())
		_err->flush();
}

qint64 QConsole::readData(char *data, qint64 maxlen)
{
	if(maxlen == 0) {
		_notifier->setEnabled(true);
		return 0;
	} else
		return _in->read(data, maxlen);
}

qint64 QConsole::writeData(const char *data, qint64 len)
{
	auto resOut = 0;
	auto resErr = 0;
	if(_writeMode.testFlag(WriteStdOut)) {
		resOut = _out->write(data, len);
		if(_writeMode == WriteStdOut)
			return resOut;
	}
	if(_writeMode.testFlag(WriteStdErr)) {
		resErr = _err->write(data, len);
		if(_writeMode == WriteStdErr)
			return resErr;
	}

	return qMin(resOut, resErr);
}

void QConsole::activated()
{
	_notifier->setEnabled(false);
	emit readyRead();
}
