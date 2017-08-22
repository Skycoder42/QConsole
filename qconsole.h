#ifndef QCONSOLE_H
#define QCONSOLE_H

#include <QIODevice>
#include <QSocketNotifier>

class QConsoleThread;
class QConsole : public QIODevice
{
	Q_OBJECT

	Q_PROPERTY(WriteMode writeMode READ writeMode WRITE setWriteMode)

public:
	enum WriteModeFlag {
		WriteStdOut = 0x01,
		WriteStdErr = 0x02,
		WriteAll = (WriteStdOut | WriteStdErr)
	};
	Q_DECLARE_FLAGS(WriteMode, WriteModeFlag)
	Q_FLAG(WriteMode)

	explicit QConsole(QObject *parent = nullptr);
	~QConsole();

	bool isSequential() const override;
	bool open(OpenMode mode = QIODevice::ReadWrite) override;
	void close() override;
	qint64 bytesAvailable() const override;

	WriteMode writeMode() const;

public slots:
	void setWriteMode(WriteMode writeMode);

	void flush();

protected:
	qint64 readData(char *data, qint64 maxlen) override;
	qint64 writeData(const char *data, qint64 len) override;

private:
	QConsoleThread *_consoleThread;
	QSocketNotifier *_notifier;
	WriteMode _writeMode;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QConsole::WriteMode)

#endif // QCONSOLE_H
