#include "qconsolethread_p.h"
#include <QDebug>
#include <iostream>

#ifdef Q_OS_UNIX
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#endif

QConsoleThread::QConsoleThread(QObject *parent) :
	QThread(parent)
{
	setTerminationEnabled(true);
}

void QConsoleThread::run()
{
//	while(!isInterruptionRequested()) {
//		timeval tv;
//		tv.tv_sec = 0;
//		tv.tv_usec = 250000;

//		fd_set rfds;
//		FD_ZERO(&rfds);
//		FD_SET(fileno(stdin), &rfds);

//		auto res = select(1, &rfds, NULL, NULL, &tv);
//		if(res == -1)
//			qWarning() << "Read failed with error" << errno;
//		else if(res)
//			qDebug() << "Data available" << res;
//		else
//			qDebug() << "No data available" << res;

////		char t;
////		std::cin >> t;
////		qDebug() << t;
//	}
}
