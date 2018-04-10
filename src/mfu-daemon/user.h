#pragma once

#include <QSet>

struct User
{
	enum Permission {
		None = 0,
		Admin = 1
	};
	int userId;
	QString username;
	QSet<Permission> permissions;
};
