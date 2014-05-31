DROP TABLE sklad CASCADE CONSTRAINTS;
DROP TABLE suroviny CASCADE CONSTRAINTS;
DROP TABLE mnozstvi_surovin CASCADE CONSTRAINTS;
DROP TABLE pecivo CASCADE CONSTRAINTS;
DROP TABLE objednavka CASCADE CONSTRAINTS;
DROP TABLE zakaznik CASCADE CONSTRAINTS;
DROP TABLE pracovnici CASCADE CONSTRAINTS;
DROP TABLE oblast CASCADE CONSTRAINTS;

 
CREATE TABLE sklad(
  id NUMBER NOT NULL,
  pocet NUMBER NOT NULL,
  id_suroviny NUMBER NOT NULL
);

CREATE TABLE suroviny(
  id NUMBER NOT NULL,
  nazev VARCHAR2(20) NOT NULL UNIQUE,
  jednotka NUMBER NOT NULL
);

CREATE TABLE mnozstvi_surovin(
  mnozstvi NUMBER NOT NULL,
  id_suroviny NUMBER NOT NULL,
  id_pecivo NUMBER NOT NULL
);

CREATE TABLE pecivo(
  id NUMBER NOT NULL,
  nazev VARCHAR2(30) NOT NULL UNIQUE,
  cena NUMBER NOT NULL,
  id_objednavka NUMBER NOT NULL
);

CREATE TABLE objednavka(
  id NUMBER NOT NULL,
  pocet NUMBER NOT NULL,
  cena NUMBER NOT NULL,
  zpusob_platby VARCHAR2(25) NOT NULL,
  datum DATE NOT NULL,
  zpusob_dodani VARCHAR2(15) NOT NULL,
  termin_dodani DATE NOT NULL,
  id_zakaznik NUMBER NOT NULL,
  id_pracovnici NUMBER NOT NULL
);

CREATE TABLE zakaznik(
  id NUMBER NOT NULL,
  jmeno VARCHAR2(20) NOT NULL,
  prijmeni VARCHAR2(30) NOT NULL, 
  adresa VARCHAR2(100) NOT NULL,
  id_oblast NUMBER NOT NULL
);

CREATE TABLE pracovnici(
  id NUMBER NOT NULL,
  jmeno VARCHAR2(20) NOT NULL,
  prijmeni VARCHAR2(30) NOT NULL,
  bydliste VARCHAR2(100) NOT NULL,
  cislo_bank_uctu VARCHAR2(20) NOT NULL UNIQUE,
  typ_zamestnance VARCHAR2(30) NOT NULL 
);

CREATE TABLE oblast(
  id NUMBER NOT NULL,
  jmeno VARCHAR2(30) NOT NULL,
  id_pracovnici NUMBER NOT NULL
);

ALTER TABLE sklad ADD CONSTRAINT PK_sklad PRIMARY KEY (id);
 
ALTER TABLE suroviny ADD CONSTRAINT PK_suroviny PRIMARY KEY (id);
 
ALTER TABLE pecivo ADD CONSTRAINT PK_pecivo PRIMARY KEY (id);
 
ALTER TABLE objednavka ADD CONSTRAINT PK_objednavka PRIMARY KEY (id);
 
ALTER TABLE zakaznik ADD CONSTRAINT PK_zakaznik PRIMARY KEY (id);

ALTER TABLE oblast ADD CONSTRAINT PK_oblast PRIMARY KEY (id);

ALTER TABLE pracovnici ADD CONSTRAINT PK_pracovnici PRIMARY KEY (id);
 
ALTER TABLE  sklad ADD CONSTRAINT FK_suroviny FOREIGN KEY(id_suroviny) REFERENCES suroviny ;
ALTER TABLE  pecivo ADD CONSTRAINT FK_objednavka FOREIGN KEY(id_objednavka) REFERENCES objednavka ;
ALTER TABLE  objednavka ADD CONSTRAINT FK_zakaznik FOREIGN KEY(id_zakaznik) REFERENCES zakaznik ;
ALTER TABLE  objednavka ADD CONSTRAINT FK_pracovnici FOREIGN KEY(id_pracovnici) REFERENCES pracovnici ;
ALTER TABLE  zakaznik ADD CONSTRAINT FK_oblast FOREIGN KEY(id_oblast) REFERENCES oblast ;
ALTER TABLE  oblast ADD CONSTRAINT FK_pracovnik FOREIGN KEY(id_pracovnici) REFERENCES pracovnici ;


INSERT INTO suroviny (id, nazev, jednotka) VALUES('001', 'mouka hladka', '11');
INSERT INTO suroviny (id, nazev, jednotka) VALUES('002', 'cukr', '20');
INSERT INTO suroviny (id, nazev, jednotka) VALUES('003', 'mouka polohruba', '13');


INSERT INTO sklad (id, pocet, id_suroviny) VALUES('001', '28', '001');
INSERT INTO sklad (id, pocet, id_suroviny) VALUES('002', '42', '003');
INSERT INTO sklad (id, pocet, id_suroviny) VALUES('003', '37', '002');

INSERT INTO pracovnici (id, jmeno, prijmeni, bydliste, cislo_bank_uctu, typ_zamestnance) VALUES('001', 'Hulmiho', 'Ukolen', 'Tvrdeho 17, Brno', '43-12345666/0100', 'boss');
INSERT INTO pracovnici (id, jmeno, prijmeni, bydliste, cislo_bank_uctu, typ_zamestnance) VALUES('002', 'Sergej', 'Stokurev', 'Palackeho 23, Blansko', '27-12665432/0100', 'styk s verejnosti');
INSERT INTO pracovnici (id, jmeno, prijmeni, bydliste, cislo_bank_uctu, typ_zamestnance) VALUES('003', 'Blanka', 'Namodrala', 'Husova 19, Rosice', '0-12345678/0800', 'ucetni');
INSERT INTO pracovnici (id, jmeno, prijmeni, bydliste, cislo_bank_uctu, typ_zamestnance) VALUES('004', 'Hang', 'Mako', 'Obla 32, Brno', '0-12333678/0800', 'ridic');
INSERT INTO pracovnici (id, jmeno, prijmeni, bydliste, cislo_bank_uctu, typ_zamestnance) VALUES('005', 'Helmut', 'Opravarkominu', 'Marie Majerove 74, Sloup', '0-12333666/0800', 'ridic');
INSERT INTO pracovnici (id, jmeno, prijmeni, bydliste, cislo_bank_uctu, typ_zamestnance) VALUES('006', 'Oje', 'Ballstrom', 'Rosickeho namesti 12, Brno', '0-12321666/0800', 'pekar');
INSERT INTO pracovnici (id, jmeno, prijmeni, bydliste, cislo_bank_uctu, typ_zamestnance) VALUES('007', 'Sandor', 'Gulemihazy', 'Lany 2, Ivancice', '0-12334566/0800', 'skladnik');

INSERT INTO oblast (id, jmeno, id_pracovnici) VALUES('634', 'Brno', '004');
INSERT INTO oblast (id, jmeno, id_pracovnici) VALUES('664', 'Oslavany', '004');
INSERT INTO oblast (id, jmeno, id_pracovnici) VALUES('679', 'Vysocany', '005');

INSERT INTO zakaznik (id, jmeno, prijmeni, adresa, id_oblast) VALUES('001', 'Berta', 'Merlinova', 'Bozetechova 2, Brno', '634');
INSERT INTO zakaznik (id, jmeno, prijmeni, adresa, id_oblast) VALUES('002', 'Aja', 'Fikova', 'Vysocany 64', '679');
INSERT INTO zakaznik (id, jmeno, prijmeni, adresa, id_oblast) VALUES('003', 'Ulrika', 'Krokova', 'V Gruntech 18, Oslavany', '664');
INSERT INTO zakaznik (id, jmeno, prijmeni, adresa, id_oblast) VALUES('004', 'Kunhuta', 'Johanova', 'Bozetechova 3, Brno', '634');

INSERT INTO objednavka (id, pocet, cena, zpusob_platby, datum, zpusob_dodani, termin_dodani, id_zakaznik,id_pracovnici) VALUES('001', '50', '1050', 'prevodem', '21/DEC/2012', 'dovoz', '03/JAN/2013', '003', '003');
INSERT INTO objednavka (id, pocet, cena, zpusob_platby, datum, zpusob_dodani, termin_dodani, id_zakaznik,id_pracovnici) VALUES('002', '200', '600', 'pri prevzeti', '21/NOV/2012', 'vyzvednuti', '08/JAN/2013', '002', '003');
INSERT INTO objednavka (id, pocet, cena, zpusob_platby, datum, zpusob_dodani, termin_dodani, id_zakaznik,id_pracovnici) VALUES('003', '70', '630', 'prevodem', '22/DEC/2012', 'dovoz', '07/FEB/2013', '001', '003');

INSERT INTO pecivo (id, nazev, cena, id_objednavka) VALUES('001', 'rohlik', '3', '002');
INSERT INTO pecivo (id, nazev, cena, id_objednavka) VALUES('002', 'chleb', '21', '001');
INSERT INTO pecivo (id, nazev, cena, id_objednavka) VALUES('003', 'kobliha', '9', '003');

INSERT INTO mnozstvi_surovin (mnozstvi, id_suroviny, id_pecivo) VALUES('23', '001', '002');
INSERT INTO mnozstvi_surovin (mnozstvi, id_suroviny, id_pecivo) VALUES('50', '003', '002');
INSERT INTO mnozstvi_surovin (mnozstvi, id_suroviny, id_pecivo) VALUES('12', '002', '003');


-- Vypise suroviny a pocet kilogramu skladem
SELECT
  suroviny.nazev,
  sklad.pocet
FROM
  suroviny,
  sklad
WHERE
  suroviny.id = sklad.id_suroviny;

-- Vypise jmena zakazniku a oblast, do ktere patri
SELECT
  zakaznik.jmeno,
  zakaznik.prijmeni,
  oblast.jmeno
FROM
  zakaznik,
  oblast
WHERE
  oblast.id = zakaznik.id_oblast;

-- Vypise vsechny objednavky vcetne ceny, jmena zakaznika a pracovnika, ktery objednavku vyrizuje
SELECT
  objednavka.cena,
  zakaznik.jmeno,
  zakaznik.prijmeni,
  pracovnici.jmeno,
  pracovnici.prijmeni
FROM
  objednavka,
  zakaznik,
  pracovnici
WHERE
  zakaznik.id (+)= objednavka.id_zakaznik
  AND
  pracovnici.id = objednavka.id_pracovnici;

-- Vypise objednavky podle ceny
SELECT
  zakaznik.prijmeni,
  objednavka.id,
  MAX(objednavka.cena)
FROM
  zakaznik,
  objednavka
WHERE
  zakaznik.id = objednavka.id_zakaznik
GROUP BY zakaznik.prijmeni, objednavka.id
ORDER BY MAX (objednavka.cena);

-- Vypise produkty podle poctu v objednavce
SELECT
  objednavka.id,
  pecivo.nazev,
  MIN(objednavka.pocet)
FROM
  pecivo,
  objednavka
WHERE
  objednavka.id = pecivo.id_objednavka
GROUP BY objednavka.id, pecivo.nazev
ORDER BY MIN (objednavka.pocet);

-- Vypise jmeno a prijmeni vsech zakazniku z oblasti 634
SELECT
  zakaznik.jmeno,
  zakaznik.prijmeni
FROM
  zakaznik
WHERE zakaznik.id IN (
  SELECT zakaznik.id
  FROM zakaznik
  WHERE zakaznik.id_oblast = '634');

-- Vypise id a jmeno oblasti, ktere spadaji pod ridice s id 004
SELECT
  oblast.id,
  oblast.jmeno
FROM
  oblast
WHERE EXISTS (
  SELECT
    pracovnici.id
  FROM
    pracovnici
  WHERE
    oblast.id_pracovnici= '004'); 
