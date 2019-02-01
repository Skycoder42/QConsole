# QConsole
A non-blocking Qt Style console class to easily handle stdin/out/err

## Features
- Based on QIODevice to allow easy integration
- Read from stdin via Qt without blocking the console
	- Uses socket notifiers on UNIX
	- Uses a parallel read thread on windows
- Shortcut methods to create QFile instances to stdin/out/err

## Installation
The package is provided via qdep, as `Skycoder42/QConsole`. To use it simply:

1. Install and enable qdep (See [qdep - Installing](https://github.com/Skycoder42/qdep#installation))
2. Add the following to your pro file:
```qmake
QDEP_DEPENDS += Skycoder42/QConsole
!load(qdep):error("Failed to load qdep feature! Run 'qdep.py prfgen --qmake $$QMAKE_QMAKE' to create it.")
```

## Example
Usage of this package is fairly simple. Create a consol instance and use it as any other QIODevice. The following is basically the demo, a small app the echoes anything you enter:

```cpp
auto out = QConsole::qStdOut(&a);
QConsole console;
QObject::connect(&console, &QConsole::readyRead, [&](){
	auto data = console.read(console.bytesAvailable());
	out->write("Echo: " + data);
	if(data.contains("exit"))
		qApp->quit();
});
console.open();
```