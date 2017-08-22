#include "readthread_win.h"
#include <QFile>
#include <QDebug>

ReadThread::ReadThread(QObject *parent) :
	QThread(parent),
	_buffer(new QBuffer(this))
{
	setTerminationEnabled(true);
}

QBuffer *ReadThread::buffer() const
{
	return _buffer;
}

void ReadThread::start()
{
	_buffer->open(QIODevice::ReadOnly);
	QThread::start();
}

void ReadThread::stop()
{
	requestInterruption();
	if(!wait(450)) {
		terminate();
		wait(50);
	}
	_buffer->close();
}

void ReadThread::run()
{
	QFile in;
	if(!in.open(stdin, QIODevice::ReadOnly | QIODevice::Unbuffered)) {
		qCritical() << "Unable to open stdin for reading with error" << in.errorString();
		return;
	}

	while(!isInterruptionRequested()) {
		if(in.error() != QFile::NoError)
			break;

		auto data = in.readLine();
		if(data.isEmpty())
			QThread::msleep(100);
		else {
			QMetaObject::invokeMethod(this, "addData", Qt::QueuedConnection,
									  Q_ARG(QByteArray, data));
		}
	}

	in.close();
	if(!isInterruptionRequested())
		qCritical() << "stdin closed unexpectedly!";
}

void ReadThread::addData(const QByteArray &data)
{
	auto &buf = _buffer->buffer();
	buf = buf.mid(_buffer->pos()) + data;
	_buffer->seek(0);
	if(buf.size() == data.size())
		emit newData();
}
