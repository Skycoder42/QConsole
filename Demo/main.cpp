#include <QCoreApplication>
#include <qconsole.h>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	QConsole console;
	QObject::connect(&console, &QConsole::readyRead, [&](){
		console.write(console.readAll());
	});
	console.open();

	return a.exec();
}
