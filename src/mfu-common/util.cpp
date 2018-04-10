#include "util.h"

#ifdef Q_OS_WIN32
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

#include <QTextStream>

namespace {
void enableEcho(bool enable = true)
{
#ifdef Q_OS_WIN32
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode;
	GetConsoleMode(hStdin, &mode);

	if( !enable )
		mode &= ~ENABLE_ECHO_INPUT;
	else
		mode |= ENABLE_ECHO_INPUT;

	SetConsoleMode(hStdin, mode );

#else
	struct termios tty;
	tcgetattr(STDIN_FILENO, &tty);
	if( !enable )
		tty.c_lflag &= ~ECHO;
	else
		tty.c_lflag |= ECHO;

	(void) tcsetattr(STDIN_FILENO, TCSANOW, &tty);
#endif
}
}

QString Util::getPasswordFromConsole(const QString & prompt)
{
	QTextStream out(stdout);
	if (!prompt.isEmpty()) {
		out << prompt;
		out.flush();
	}

	QString password;
	enableEcho(false);
	QTextStream in(stdin);
	password = in.readLine();
	enableEcho(true);

	out << endl;

	return password;
}
