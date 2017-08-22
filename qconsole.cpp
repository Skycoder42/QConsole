#include "qconsole.h"
#include <QDebug>
#include <QFile>
#ifdef Q_OS_WIN
#include "readthread_win.h"
#else
#include <sys/ioctl.h>
#include <errno.h>
#endif

QConsole::QConsole(QObject *parent) :
	QIODevice(parent),
#ifdef Q_OS_WIN
	_readThread(new ReadThread(this))
#else
	_notifier(new QSocketNotifier(0, QSocketNotifier::Read, this)),
	_in(new QFile(this))
#endif
{
#ifdef Q_OS_WIN
	connect(_readThread, &ReadThread::newData,
			this, &QConsole::readyRead);
#else
	_in->setObjectName(QStringLiteral("stdin"));
	_notifier->setEnabled(false);
	connect(_notifier, &QSocketNotifier::activated,
			this, &QConsole::activated);
#endif
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

bool QConsole::open()
{
	if(QIODevice::open(ReadOnly | Unbuffered)) {
#ifdef Q_OS_WIN
		_readThread->start();
#else
		if(!_in->open(stdin, ReadOnly | Unbuffered)) {
			close();
			setErrorString(QStringLiteral("stdin: %2").arg(_in->errorString()));
			return false;
		}
		_notifier->setEnabled(true);
#endif
		return true;
	} else
		return false;
}

void QConsole::close()
{
	if(isOpen()) {
#ifdef Q_OS_WIN
		_readThread->stop();
#else
		_notifier->setEnabled(false);
		if(_in->isOpen())
			_in->close();
#endif
	}
	QIODevice::close();
}

qint64 QConsole::bytesAvailable() const
{
#ifdef Q_OS_WIN
	return QIODevice::bytesAvailable() + _readThread->buffer()->bytesAvailable();
#else
	int n = -1;
	if (ioctl(0, FIONREAD, &n) < 0) {
		qWarning() << "ioctl failed with error" << errno;
		return 0;
	}
	return QIODevice::bytesAvailable() + (qint64)n;
#endif
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
#ifdef Q_OS_WIN
	auto res = _readThread->buffer()->read(data, maxlen);
#else
	auto res = _in->read(data, maxlen);
	_notifier->setEnabled(true);
#endif
	return res;
}

qint64 QConsole::writeData(const char *data, qint64 len)
{
	Q_UNUSED(data)
	Q_UNUSED(len)
	return 0;
}

#ifdef Q_OS_UNIX
void QConsole::activated()
{
	_notifier->setEnabled(false);
	emit readyRead();
}
#endif

bool QConsole::open(QIODevice::OpenMode openMode)
{
	Q_UNUSED(openMode)
	return open();
}
