/*
 Navicat Premium Dump SQL

 Source Server         : book
 Source Server Type    : SQLite
 Source Server Version : 3045000 (3.45.0)
 Source Schema         : main

 Target Server Type    : SQLite
 Target Server Version : 3045000 (3.45.0)
 File Encoding         : 65001

 Date: 15/01/2026 01:36:26
*/

PRAGMA foreign_keys = false;

-- ----------------------------
-- Table structure for book
-- ----------------------------
DROP TABLE IF EXISTS "book";
CREATE TABLE "book" (
  "bookid" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
  "name" TEXT,
  "press" TEXT,
  "type1" TEXT,
  "type2" TEXT,
  "type3" TEXT,
  "cnt" integer,
  "pic" TEXT
);

-- ----------------------------
-- Records of book
-- ----------------------------
INSERT INTO "book" VALUES (1, '水浒传', '29.9', '历史', '人文', NULL, 100, NULL);
INSERT INTO "book" VALUES (2, '三国演义', '29.9', '历史', '人文', NULL, 100, NULL);
INSERT INTO "book" VALUES (3, '西游记', '29.9', '历史', '人文', NULL, 100, NULL);
INSERT INTO "book" VALUES (4, '红楼梦', '29.9', '历史', '人文', NULL, 100, NULL);

-- ----------------------------
-- Table structure for record
-- ----------------------------
DROP TABLE IF EXISTS "record";
CREATE TABLE "record" (
  "id" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
  "bookid" INTEGER NOT NULL,
  "userid" INTEGER,
  "start" TEXT,
  "end" TEXT,
  FOREIGN KEY ("bookid") REFERENCES "book" ("bookid") ON DELETE NO ACTION ON UPDATE NO ACTION,
  FOREIGN KEY ("userid") REFERENCES "user" ("userid") ON DELETE NO ACTION ON UPDATE NO ACTION
);

-- ----------------------------
-- Records of record
-- ----------------------------
INSERT INTO "record" VALUES (1, 1, 2, '2026年1月1日10:30:00', '2026年1月2日10:50:00');

-- ----------------------------
-- Table structure for sqlite_sequence
-- ----------------------------
DROP TABLE IF EXISTS "sqlite_sequence";
CREATE TABLE "sqlite_sequence" (
  "name",
  "seq"
);

-- ----------------------------
-- Records of sqlite_sequence
-- ----------------------------
INSERT INTO "sqlite_sequence" VALUES ('user', 21);
INSERT INTO "sqlite_sequence" VALUES ('book', 4);
INSERT INTO "sqlite_sequence" VALUES ('record', 1);

-- ----------------------------
-- Table structure for user
-- ----------------------------
DROP TABLE IF EXISTS "user";
CREATE TABLE "user" (
  "userid" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
  "username" TEXT,
  "password" TEXT,
  "nickname" TEXT,
  "auth" TEXT,
  "department" TEXT,
  "grade" TEXT
);

-- ----------------------------
-- Records of user
-- ----------------------------
INSERT INTO "user" VALUES (1, '23级3班', '软件工程', '学生', '六六', '123456', 'liuliu');
INSERT INTO "user" VALUES (2, '23级3班', '软件工程', '学生', '七七', '123456', 'qiqi');
INSERT INTO "user" VALUES (3, '23级4班', '软件工程', '学生', '八八', '123456', 'baba');
INSERT INTO "user" VALUES (4, '23级4班', '软件工程', '学生', '九九', '123456', 'jiujiu');
INSERT INTO "user" VALUES (5, '23级3班', '软件工程', '学生', '十十', '123456', 'shishi');
INSERT INTO "user" VALUES (6, '23级3班', '软件工程', '学生', '一一', '123456', 'yiyi');
INSERT INTO "user" VALUES (7, '23级1班', '软件工程', '学生', '二二', '123456', 'erer');

-- ----------------------------
-- Auto increment value for book
-- ----------------------------
UPDATE "sqlite_sequence" SET seq = 4 WHERE name = 'book';

-- ----------------------------
-- Auto increment value for record
-- ----------------------------
UPDATE "sqlite_sequence" SET seq = 1 WHERE name = 'record';

-- ----------------------------
-- Auto increment value for user
-- ----------------------------
UPDATE "sqlite_sequence" SET seq = 21 WHERE name = 'user';

PRAGMA foreign_keys = true;
