-- This scheme only works for sqlite databases (pragmas, no innodb engine, autoincrement in a different way,...)

-- VERSION 1

CREATE TABLE `clients` (
  `macAddress` varchar NOT NULL,
  `name`       varchar DEFAULT NULL,
  `location`   varchar DEFAULT NULL,
  `streamUrl`  varchar DEFAULT NULL,
  `streamPort` INT DEFAULT NULL,
  `registered` BOOL DEFAULT 0,
  PRIMARY KEY (macAddress)
);

-- VERSION 2

PRAGMA foreign_keys = ON;

CREATE TABLE `users` (
  `userId`       INTEGER PRIMARY KEY AUTOINCREMENT,
  `login`        varchar NOT NULL,
  `passwordHash` varchar NOT NULL
);

CREATE TABLE `permissions` (
  `userId`     INTEGER,
  `permission` INTEGER,
  PRIMARY KEY (userId, permission),
  FOREIGN KEY(userId) REFERENCES users(userId)
);
