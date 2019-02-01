#ifndef READTHREAD_WIN_H
#define READTHREAD_WIN_H

#include <QtCore/QThread>
#include <QtCore/QBuffer>

class ReadThread : public QThread
{
	Q_OBJECT

public:
	ReadThread(QObject *parent = nullptr);

	QBuffer *buffer() const;
	void start();
	void stop();

Q_SIGNALS:
	void newData();

protected:
	void run() override;

private Q_SLOTS:
	void addData(const QByteArray &data);

private:
	QBuffer *_buffer;
};

#endif // READTHREAD_WIN_H
