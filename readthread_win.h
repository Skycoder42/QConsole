#ifndef READTHREAD_WIN_H
#define READTHREAD_WIN_H

#include <QThread>
#include <QBuffer>

class ReadThread : public QThread
{
	Q_OBJECT

public:
	ReadThread(QObject *parent = nullptr);

	QBuffer *buffer() const;
	void start();
	void stop();

signals:
	void newData();

protected:
	void run() override;

private slots:
	void addData(const QByteArray &data);

private:
	QBuffer *_buffer;
};

#endif // READTHREAD_WIN_H
