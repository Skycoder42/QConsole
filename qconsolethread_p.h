#ifndef QCONSOLETHREAD_P_H
#define QCONSOLETHREAD_P_H

#include <QThread>

class QConsoleThread : public QThread
{
	Q_OBJECT

public:
	explicit QConsoleThread(QObject *parent = nullptr);

protected:
	void run() override;
};

#endif // QCONSOLETHREAD_P_H
