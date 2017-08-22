#include "qconsole.h"
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
	_in(new QFile(this))
{
	_in->setObjectName(QStringLiteral("stdin"));
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

bool QConsole::open()
{
	if(QIODevice::open(ReadOnly | Unbuffered)) {

		if(!_in->open(stdin, ReadOnly | Unbuffered)) {
			close();
			setErrorString(QStringLiteral("stdin: %2").arg(_in->errorString()));
			return false;
		}
		_notifier->setEnabled(true);
#ifdef Q_OS_WIN
		FlushConsoleInputBuffer(_notifier->handle());
#endif
		return true;
	} else
		return false;
}

void QConsole::close()
{
	if(isOpen()) {
		_notifier->setEnabled(false);
		if(_in->isOpen())
			_in->close();
	}
	QIODevice::close();
}

qint64 QConsole::bytesAvailable() const
{
	return QIODevice::bytesAvailable() + _in->bytesAvailable();
}

QFile *QConsole::qStdOut(QObject *parent)
{
	auto file = new QFile(parent);
	file->open(stdout, QIODevice::WriteOnly | QIODevice::Unbuffered);
	return file;
}

QFile *QConsole::qStdErr(QObject *parent)
{
	auto file = new QFile(parent);
	file->open(stderr, QIODevice::WriteOnly | QIODevice::Unbuffered);
	return file;
}

qint64 QConsole::readData(char *data, qint64 maxlen)
{
	if(maxlen == 0)
		return 0;
	auto res = _in->read(data, maxlen);
	_notifier->setEnabled(true);
	return res;
}

qint64 QConsole::writeData(const char *data, qint64 len)
{
	Q_UNUSED(data)
	Q_UNUSED(len)
	return 0;
}

void QConsole::activated()
{
	_notifier->setEnabled(false);
	emit readyRead();
}

bool QConsole::open(QIODevice::OpenMode openMode)
{
	Q_UNUSED(openMode)
	return open();
}
