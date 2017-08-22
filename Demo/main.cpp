#include <QCoreApplication>
#include <QDebug>
#include <qconsole.h>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	QConsole console;
	QObject::connect(&console, &QConsole::readyRead, [&](){
		auto data = console.readLine();
		qDebug() << data;
		if(data.contains("exit"))
			qApp->quit();
	});
	if(!console.open()) {
		qCritical() << console.errorString();
		return EXIT_FAILURE;
	}
	return a.exec();
}
