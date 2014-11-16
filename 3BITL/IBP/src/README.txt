Uzivatelska napoveda k programu Liveness detection tool

Program se spousti nasledujicim zpusobem:
python detect.py [nazev_slozky]

Nazev slozky je nepovinny parametr. Pokud neni slozka zadana, program nacita snimky ze slozky "samples".

Po zpracovani kazdeho snimku je vypsan nazev snimku a informace, zda byl prst prijat jako zivy ("ACCEPTED") nebo odmitnut jako nezivy ("REJECTED"). Program ocekava validni vstupy.

Jsou dostupne 4 slozky se snimky:
samples/ - obsahuje jeden snimek ziveho prstu a jeden snimek neziveho prstu, slouzi pouze pro rychlou ukazku funkcionality
samples_fake/ - obsahuje 64 snimku nezivych prstu, ktere byly pouzity pro detekci zivosti v ramci bakalarske prace
samples_live/ - obsahuje 88 snimku zivych prstu, ktere byly pouzity pro detekci zivosti v ramci bakalarske prace
samples_uncut/ - slouzi pouze pro doplneni (ne pro testovani programu), obsahuje vsechny snimky ze slozek samples_fake/ a samples_live/ pred oriznutim stupnice s teplotou
