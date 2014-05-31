-- Adminer 3.6.1 MySQL dump

SET NAMES utf8;
SET foreign_key_checks = 0;
SET time_zone = 'SYSTEM';
SET sql_mode = 'NO_AUTO_VALUE_ON_ZERO';

DROP TABLE IF EXISTS `obj_pecivo`;
CREATE TABLE `obj_pecivo` (
  `objednavka` int(5) unsigned NOT NULL,
  `pecivo` int(5) unsigned NOT NULL,
  `pocet` int(5) unsigned NOT NULL,
  `id` int(5) unsigned NOT NULL AUTO_INCREMENT,
  PRIMARY KEY (`id`),
  KEY `pecivo` (`pecivo`),
  KEY `objednavka` (`objednavka`),
  CONSTRAINT `obj_pecivo_ibfk_2` FOREIGN KEY (`pecivo`) REFERENCES `pecivo` (`id`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `obj_pecivo_ibfk_3` FOREIGN KEY (`objednavka`) REFERENCES `objednavka` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=14 DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci;

INSERT INTO `obj_pecivo` (`objednavka`, `pecivo`, `pocet`, `id`) VALUES
(1,	2,	5,	1),
(1,	3,	10,	2),
(3,	5,	5,	3)
ON DUPLICATE KEY UPDATE `objednavka` = VALUES(`objednavka`), `pecivo` = VALUES(`pecivo`), `pocet` = VALUES(`pocet`), `id` = VALUES(`id`);

DROP TABLE IF EXISTS `objednavka`;
CREATE TABLE `objednavka` (
  `id` int(5) unsigned NOT NULL AUTO_INCREMENT,
  `zpusob_platby` int(10) unsigned NOT NULL,
  `datum` date NOT NULL,
  `zpusob_dodani` int(11) NOT NULL,
  `id_zakaznik` int(50) unsigned NOT NULL,
  `id_pracovnik` int(50) unsigned DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `id_pracovnik` (`id_pracovnik`),
  KEY `id_zakaznik` (`id_zakaznik`),
  CONSTRAINT `objednavka_ibfk_3` FOREIGN KEY (`id_pracovnik`) REFERENCES `pracovnik` (`id`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `objednavka_ibfk_4` FOREIGN KEY (`id_zakaznik`) REFERENCES `zakaznik` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=20 DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci;

INSERT INTO `objednavka` (`id`, `zpusob_platby`, `datum`, `zpusob_dodani`, `id_zakaznik`, `id_pracovnik`) VALUES
(1,	0,	'2012-12-10',	1,	1,	1),
(3,	1,	'2012-12-10',	0,	1,	2),
(4,	1,	'2012-12-10',	0,	3,	1),
(5,	1,	'2012-12-10',	0,	1,	3),
(6,	1,	'2012-12-10',	1,	3,	4),
(7,	1,	'2012-12-10',	0,	1,	9),
(13,	0,	'0000-00-00',	0,	1,	NULL),
(14,	0,	'0000-00-00',	0,	1,	NULL),
(15,	0,	'0000-00-00',	0,	1,	NULL),
(16,	0,	'0000-00-00',	0,	1,	NULL),
(17,	0,	'0000-00-00',	0,	10,	NULL),
(18,	0,	'0000-00-00',	0,	10,	NULL),
(19,	0,	'0000-00-00',	0,	10,	NULL)
ON DUPLICATE KEY UPDATE `id` = VALUES(`id`), `zpusob_platby` = VALUES(`zpusob_platby`), `datum` = VALUES(`datum`), `zpusob_dodani` = VALUES(`zpusob_dodani`), `id_zakaznik` = VALUES(`id_zakaznik`), `id_pracovnik` = VALUES(`id_pracovnik`);

DROP TABLE IF EXISTS `oblast`;
CREATE TABLE `oblast` (
  `id` int(5) unsigned NOT NULL AUTO_INCREMENT,
  `jmeno` varchar(30) CHARACTER SET utf8 COLLATE utf8_czech_ci NOT NULL,
  `id_pracovnik` int(5) unsigned DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `id_pracovnik` (`id_pracovnik`),
  CONSTRAINT `oblast_ibfk_3` FOREIGN KEY (`id_pracovnik`) REFERENCES `pracovnik` (`id`) ON DELETE NO ACTION ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=11 DEFAULT CHARSET=latin2 COLLATE=latin2_czech_cs;

INSERT INTO `oblast` (`id`, `jmeno`, `id_pracovnik`) VALUES
(1,	'Oz',	3),
(2,	'Narnie',	NULL),
(3,	'Stredozem',	4),
(4,	'Oslavany',	NULL),
(5,	'Rosice',	NULL),
(6,	'Blansko',	NULL),
(7,	'Boskovice',	NULL),
(8,	'Vysoèany',	NULL)
ON DUPLICATE KEY UPDATE `id` = VALUES(`id`), `jmeno` = VALUES(`jmeno`), `id_pracovnik` = VALUES(`id_pracovnik`);

DROP TABLE IF EXISTS `pecivo`;
CREATE TABLE `pecivo` (
  `id` int(5) unsigned NOT NULL AUTO_INCREMENT,
  `nazev` varchar(50) COLLATE utf8_czech_ci NOT NULL,
  `cena` int(5) unsigned NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=12 DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci;

INSERT INTO `pecivo` (`id`, `nazev`, `cena`) VALUES
(1,	'chleba',	20),
(2,	'koblizek kakaový',	9),
(3,	'koblizek jahodový',	7),
(4,	'buchta maková',	6),
(5,	'buchta tvarohová',	9),
(6,	'buchta povidlová',	8),
(7,	'rohlik',	2),
(8,	'kaiserka sezamová',	4),
(9,	'kaiserka tmavá',	4),
(10,	'vánocka',	25),
(11,	'chleba toustový',	27)
ON DUPLICATE KEY UPDATE `id` = VALUES(`id`), `nazev` = VALUES(`nazev`), `cena` = VALUES(`cena`);

DROP TABLE IF EXISTS `pracovnik`;
CREATE TABLE `pracovnik` (
  `id` int(5) unsigned NOT NULL AUTO_INCREMENT,
  `jmeno` varchar(25) CHARACTER SET utf8 COLLATE utf8_czech_ci NOT NULL,
  `prijmeni` varchar(30) CHARACTER SET utf8 COLLATE utf8_czech_ci NOT NULL,
  `bydliste` varchar(100) CHARACTER SET utf8 COLLATE utf8_czech_ci NOT NULL,
  `cislo_bu` varchar(20) CHARACTER SET utf8 COLLATE utf8_czech_ci NOT NULL,
  `typ_zam` int(5) unsigned NOT NULL,
  `heslo` varchar(15) CHARACTER SET utf8 COLLATE utf8_czech_ci DEFAULT NULL,
  `login` varchar(15) CHARACTER SET utf8 COLLATE utf8_czech_ci DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `login` (`login`)
) ENGINE=InnoDB AUTO_INCREMENT=17 DEFAULT CHARSET=latin2 COLLATE=latin2_czech_cs;

INSERT INTO `pracovnik` (`id`, `jmeno`, `prijmeni`, `bydliste`, `cislo_bu`, `typ_zam`, `heslo`, `login`) VALUES
(1,	'Franta',	'Vocasek',	'Vidlakov 42',	'1234567891/0000',	1,	'heslo',	'admin'),
(2,	'Ikarus',	'Padly',	'Nebe 12',	'2345678987/3456',	2,	'heslo',	'zamestnanec'),
(3,	'Pytel',	'Pisku',	'Kamenolom 20',	'9876543219/0000',	3,	NULL,	NULL),
(4,	'Kravík',	'Kakaový',	'Hory 22',	'9876543981/0000',	3,	NULL,	NULL),
(9,	'Sergej',	'Stokurev',	'Moulin Rouge',	'9876543219/0001',	3,	NULL,	NULL)
ON DUPLICATE KEY UPDATE `id` = VALUES(`id`), `jmeno` = VALUES(`jmeno`), `prijmeni` = VALUES(`prijmeni`), `bydliste` = VALUES(`bydliste`), `cislo_bu` = VALUES(`cislo_bu`), `typ_zam` = VALUES(`typ_zam`), `heslo` = VALUES(`heslo`), `login` = VALUES(`login`);

DROP TABLE IF EXISTS `zakaznik`;
CREATE TABLE `zakaznik` (
  `id` int(5) unsigned NOT NULL AUTO_INCREMENT,
  `jmeno` varchar(25) COLLATE utf8_czech_ci NOT NULL,
  `login` varchar(25) COLLATE utf8_czech_ci NOT NULL,
  `heslo` varchar(25) COLLATE utf8_czech_ci NOT NULL,
  `prijmeni` varchar(30) COLLATE utf8_czech_ci NOT NULL,
  `adresa` varchar(100) COLLATE utf8_czech_ci NOT NULL,
  `id_oblast` int(5) unsigned DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `id_oblast` (`id_oblast`),
  CONSTRAINT `zakaznik_ibfk_1` FOREIGN KEY (`id_oblast`) REFERENCES `oblast` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=11 DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci;

INSERT INTO `zakaznik` (`id`, `jmeno`, `login`, `heslo`, `prijmeni`, `adresa`, `id_oblast`) VALUES
(1,	'Frodo',	'zakaznik',	'heslo',	'Pytlik',	'Hobitin 22',	1),
(3,	'Franta',	'bla',	'ble',	'Suchanek',	'Praha 4',	1),
(10,	'Bozena',	'bozka',	'bozenka',	'Hlavata',	'Na konci sveta 57',	NULL)
ON DUPLICATE KEY UPDATE `id` = VALUES(`id`), `jmeno` = VALUES(`jmeno`), `login` = VALUES(`login`), `heslo` = VALUES(`heslo`), `prijmeni` = VALUES(`prijmeni`), `adresa` = VALUES(`adresa`), `id_oblast` = VALUES(`id_oblast`);

-- 2012-12-10 23:42:05
