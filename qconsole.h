#ifndef QCONSOLE_H
#define QCONSOLE_H

#include <QIODevice>
class QFile;
#ifdef Q_OS_WIN
class ReadThread;
#else
#include <QSocketNotifier>
#endif

class QConsole : public QIODevice
{
	Q_OBJECT

public:
	explicit QConsole(QObject *parent = nullptr);
	~QConsole();

	bool isSequential() const override;
	bool open();
	void close() override;
	qint64 bytesAvailable() const override;

	static QFile *qStdOut(QObject *parent = nullptr);
	static QFile *qStdErr(QObject *parent = nullptr);
	static QFile *qStdIn(QObject *parent = nullptr);

protected:
	qint64 readData(char *data, qint64 maxlen) override;
	qint64 writeData(const char *data, qint64 len) override;

#ifdef Q_OS_UNIX
private slots:
	void activated();
#endif

private:
#ifdef Q_OS_WIN
	ReadThread *_readThread;
#else
	QSocketNotifier *_notifier;
	QFile *_in;
#endif

	bool open(OpenMode openMode) override;
};

#endif // QCONSOLE_H
