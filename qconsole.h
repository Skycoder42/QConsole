#ifndef QCONSOLE_H
#define QCONSOLE_H

#include <QtCore/QIODevice>
class QFile;
#ifdef Q_OS_WIN
class ReadThread;
#else
#include <QtCore/QSocketNotifier>
#endif

class Q_CONSOLE_EXPORT QConsole : public QIODevice
{
	Q_OBJECT

public:
	explicit QConsole(QObject *parent = nullptr);
	~QConsole() override;

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
private Q_SLOTS:
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

#ifdef Q_OS_UNIX
	qint64 nBytes() const;
#endif
};

#endif // QCONSOLE_H
