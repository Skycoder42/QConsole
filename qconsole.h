#ifndef QCONSOLE_H
#define QCONSOLE_H

#include <QIODevice>
class QFile;
#ifdef Q_OS_WIN
#include <QWinEventNotifier>
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

protected:
	qint64 readData(char *data, qint64 maxlen) override;
	qint64 writeData(const char *data, qint64 len) override;

private slots:
	void activated();

private:
#ifdef Q_OS_WIN
	QWinEventNotifier *_notifier;
#else
	QSocketNotifier *_notifier;
#endif
	QFile *_in;

	bool
	open(OpenMode openMode) override;
};

#endif // QCONSOLE_H
