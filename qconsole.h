#ifndef QCONSOLE_H
#define QCONSOLE_H

#include <QIODevice>
class QFile;
class QBuffer;
#ifdef Q_OS_WIN
#include <QWinEventNotifier>
#else
#include <QSocketNotifier>
#endif

class QConsole : public QIODevice
{
	Q_OBJECT

public:
	enum WriteModeFlag {
		WriteNone = 0x00,
		WriteStdOut = 0x01,
		WriteStdErr = 0x02,
		WriteAll = (WriteStdOut | WriteStdErr)
	};
	Q_DECLARE_FLAGS(WriteMode, WriteModeFlag)
	Q_FLAG(WriteMode)

	explicit QConsole(QObject *parent = nullptr);
	~QConsole();

	bool isSequential() const override;
	bool open(OpenMode openMode = ReadWrite) override;
	bool open(OpenMode openMode, WriteMode writeMode);
	void close() override;
	qint64 bytesAvailable() const override;

	WriteMode writeMode() const;

public slots:
	void flush();

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
	QFile *_out;
	QFile *_err;
	QBuffer *_buffer;
	WriteMode _writeMode;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QConsole::WriteMode)

#endif // QCONSOLE_H
