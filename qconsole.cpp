#include "qconsole.h"
#include <QDebug>
#include <QFile>
#ifdef Q_OS_WIN
#include "readthread_win.h"
#else
#include <sys/ioctl.h>
#endif

QConsole::QConsole(QObject *parent) :
	QIODevice(parent),
#ifdef Q_OS_WIN
	_readThread{new ReadThread{this}}
#else
	_notifier{new QSocketNotifier{fileno(stdin), QSocketNotifier::Read, this}},
	_in{new QFile{this}}
#endif
{
#ifdef Q_OS_WIN
	connect(_readThread, &ReadThread::newData,
			this, &QConsole::readyRead);
	connect(_readThread, &ReadThread::eofTriggered,
			this, &QConsole::readChannelFinished);
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
		QConsole::close();
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
	return QIODevice::bytesAvailable() + nBytes();
#endif
}

QFile *QConsole::qStdOut(QObject *parent)
{
	auto file = new QFile{parent};
	file->open(stdout, QIODevice::WriteOnly | QIODevice::Unbuffered);
	return file;
}

QFile *QConsole::qStdErr(QObject *parent)
{
	auto file = new QFile{parent};
	file->open(stderr, QIODevice::WriteOnly | QIODevice::Unbuffered);
	return file;
}

QFile *QConsole::qStdIn(QObject *parent)
{
	auto file = new QFile{parent};
	file->open(stdin, QIODevice::ReadOnly | QIODevice::Unbuffered);
	return file;
}

qint64 QConsole::readData(char *data, qint64 maxlen)
{
	if(maxlen == 0)
		return 0;
#ifdef Q_OS_WIN
	return _readThread->buffer()->read(data, maxlen);
#else
	auto res = _in->read(data, maxlen);
	_notifier->setEnabled(true);
	return res;
#endif
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
	if(nBytes() <= 0)
		emit readChannelFinished();
	else
		emit readyRead();
}
#endif

bool QConsole::open(QIODevice::OpenMode openMode)
{
	Q_UNUSED(openMode)
	return open();
}

#ifdef Q_OS_UNIX
qint64 QConsole::nBytes() const
{
	int n = -1;
	if (ioctl(0, FIONREAD, &n) < 0) {
		qWarning().noquote() << "ioctl failed with error:"
							 << qt_error_string();
		return -1;
	} else
		return static_cast<qint64>(n);
}
#endif
