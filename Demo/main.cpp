#include <QCoreApplication>
#include <QDebug>
#include <qconsole.h>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	QConsole console;
	QObject::connect(&console, &QConsole::readyRead, [&](){
		auto data = console.readLine();
		console.write(data);
		console.flush();
		if(data.contains("exit"))
			qApp->quit();
	});
	if(!console.open(QIODevice::ReadWrite, QConsole::WriteAll)) {
		qCritical() << console.errorString();
		return EXIT_FAILURE;
	}
	console.write("Hello\n");

	return a.exec();
}
