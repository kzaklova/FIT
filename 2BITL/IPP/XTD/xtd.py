#!/usr/bin/env python3

#XTD:xzaklo00

import sys
import argparse
from xml.dom.minidom import parse

# vycet datovych typu pro tabulky
(BIT, INT, FLOAT, STRING, NVARCHAR, NTEXT) = range(1, 7)

# zpracovani parametru
def parseArgs():
    parser = argparse.ArgumentParser(description='Projekt do predmetu IPP', add_help=False)
    parser.add_argument('-h','--help', action='count', default= 0, help='vypise napovedu')
    parser.add_argument('--input', dest="infile", action="append", default= [], help='vstupni soubor ve formatu XML, popr. stdin')
    parser.add_argument('--output', dest="outfile", action="append", default= [], help='vystupni soubor popr. stdout')
    parser.add_argument('--header', dest="header", action="append", default= [], help='na zacatek vystupu vlozi zakomentovanou hlavicku')
    parser.add_argument('--etc', dest="etc", action="append", default= [], help='urcuje maximalni pocet sloupcu vzniklych ze stejnojmennych podelementu')
    parser.add_argument('-a', action="count", default= 0, help='ve vystupnim XML se nebudou generovat sloupce z atributu')
    parser.add_argument('-b', action="count", default= 0, help='pokud element obsahuje vice podelementu stejneho nazvu, uvazuje se pouze jeden z nich, ten s nejvyssim datovym typem, nesmi byt kombinovano s parametrem --etc')
    parser.add_argument('-g', action="count", default= 0, help='lze jej uplatnit s jakymkoliv prepinacem krome --help, generuje XML, kde vypise vsechny tabulky a vztahy mezi nimi')
    parser.add_argument('--isvalid', dest="isvalid", action="append", default= [], help='vstupni soubor k overeni ve formatu XML')

    try:
        args = parser.parse_args()
    except:
        print('Chyba v argumentech', file=sys.stderr)
        sys.exit(1)
    if (len(args.infile) > 1):
        print('Vstupni soubor muze byt pouze jeden', file=sys.stderr)
        sys.exit(1)
    if (len(args.outfile) > 1):
        print('Vystupni soubor muze byt pouze jeden', file=sys.stderr)
        sys.exit(1)
    if (len(args.header) > 1):
        print('Vicenasobna hlavicka', file=sys.stderr)
        sys.exit(1)
    if (len(args.etc) > 1):
        print('Vicekrat zadano etc', file=sys.stderr)
        sys.exit(1)
    if (len(args.isvalid) > 1):
        print('Validovat lze jen jednou', file=sys.stderr)
        sys.exit(1)
    if ((args.a > 1) or (args.b > 1) or (args.g > 1)):
        print('Vicenasobne zadany parametr', file=sys.stderr)
        sys.exit(1)
    if ((args.b > 0) and (len(args.etc) > 0)):
        print('-b a --etc nesmi byt zadany dohromady', file=sys.stderr)
        sys.exit(1)
    if (args.help == 1):
        if len(sys.argv) == 2:
            parser.print_help()
            sys.exit(0)
        else:
            print('--help a jiny argument', file=sys.stderr)
            sys.exit(1)
    if (args.help > 1):
        print('--help staci zadat jen jednou', file=sys.stderr)
        sys.exit(1)
    return args

# funkce pro zjisteni datoveho typu elementu
def my_type(var):
    if (var == '1') or (var == '0') or (var == 'True') or (var == 'False') or (var == ''):
        return BIT
    try:
        int(var)
        return INT
    except:
        pass
    try:
        float(var)
        return FLOAT
    except:
        pass
    return STRING

# funkce navrati retezec podle datoveho typu elementu
def ret_type(var):
    if var == BIT:
        return "BIT"
    if var == INT:
        return "INT"
    if var == FLOAT:
        return "FLOAT"
    if var == NVARCHAR:
        return "NVARCHAR"
    if var == NTEXT:
        return "NTEXT"

# parsovani vstupniho XML
def parse_xml(all_tables, all_attrs, params, root, aux):
    # pomocny slovnik pro odkazy na jine tabulky
    fkeys= {}
    # zpracovani bezprostrednich potomku root uzlu
    for node in aux.childNodes:
        # aktualni uzel
        current = node.nodeName.lower()
        # rodic aktualniho uzlu
        parent = node.parentNode.nodeName.lower()
        # jedna se o element
        if node.nodeType == 1:
            # uzel jeste neni ulozeny, vytvorime mu zaznam
            if current not in all_tables.keys():
                all_tables[current] = {}
                all_attrs[current] = {}
            # generujeme atributy
            if (params.a == 0):
                for i in range(len(node.attributes)):
                    # jmeno aktualniho atributu
                    a_name = node.attributes.item(i).name.lower()
                    # zjisteni datoveho typu
                    all_attrs [current] [a_name] = my_type(node.attributes.item(i).value)
                    if all_attrs [current] [a_name] == STRING:
                        all_attrs [current] [a_name] = NVARCHAR
            # zpracovani podelementu a poctu jeho vyskytu
            if aux.nodeName != root.nodeName:
                if current not in fkeys.keys():
                    fkeys[current] = 1
                else:
                    fkeys[current] += 1
            parse_xml(all_tables, all_attrs, params, root, node)
        # jedna se o text
        elif node.nodeType == 3:
            if node.data.strip() != "":
                # zpracujeme prvek a zaznacime si jeho datovy typ
                if aux.nodeName != root.nodeName:
                    t = my_type(node.data)
                    if t == STRING:
                        t = NTEXT
                    if "value" in all_attrs [parent]:
                        # pokud ma nektery vyskyt prvku vyssi datovy typ, prepiseme stavajici
                        if all_attrs [parent]["value"] < t:
                            all_attrs[parent]["value"] = t
                    else:
                        all_attrs[parent]["value"] = t
        if aux.nodeName != root.nodeName:
            # ulozime pocty vyskytu podelementu
            for item in fkeys:
                if item not in all_tables [parent]:
                    all_tables [parent][item] = fkeys[item]
                else:
                    if all_tables[parent][item] < fkeys[item]:
                        all_tables[parent][item] = fkeys[item]

# funkce pro kontrolu kofliktu v nazvech
def conflict(all_tables, all_attrs):
    for item in all_tables:
        for i in all_tables[item]:
            # PK a FK
            if "prk_"+item+"id" == i+"_id":
                print('Konflikt primarniho klice a ciziho klice', file=sys.stderr)
                sys.exit(90)
            for j in all_attrs[item]:
                # PK a atribut
                if "prk_"+item+"id" == j:
                    print('Konflikt primarniho klice a atributu', file=sys.stderr)
                    sys.exit(90)
                # FK a atribut
                if i+"_id" == j:
                    print('Konflikt ciziho klice a atributu', file=sys.stderr)
                    sys.exit(90)

# vypis tabulky na vystup
def print_table(params, f, all_tables, all_attrs):
    # mame tisknout hlavicku
    if len(params.header) == 1:
        f.write("--"+params.header[0]+"\n\n")
    for item in all_tables:
        # tabulka + PK
        f.write("CREATE TABLE "+item+"(\n  prk_"+item+"_id INT PRIMARY KEY")
        # FK
        for i in all_tables[item]:
            f.write(",\n  "+i+"_id INT")
        # atributy
        for i in all_attrs[item]:
            f.write(",\n  "+i+" "+ret_type(all_attrs[item][i]))
        f.write("\n);\n\n")

# pocitadlo pro vice odkazu na jinou tabulku
def set_counter(all_tables):
    aux = []
    aux2 =[]
    for item in all_tables:
        for i in all_tables[item]:
            # existuje vice nez 1 odkaz
            if all_tables[item][i] > 1:
                for count in range(1, all_tables[item][i]+1):
                    aux2.append(i+str(count))
                aux.append(i)
        # smazeme puvodni odkaz
        for member in aux:
            del all_tables[item][member]
        # vlozime cislovane odkazy 
        for member in aux2:
            all_tables[item][member] = 1
        aux = []
        aux2 = []

# zpracovani parametru --etc=num
def my_etc (num, all_tables, all_attrs):
    d = []
    for item in all_tables:
        for i in all_tables[item]:
            # hodnota odkazu je vyssi nez cislo v etc
            if all_tables[item][i] > num:
                # "prohodime" odkaz
                all_tables[i][item] = -1
                d.append(i)
        # smazeme puvodni odkaz
        for member in d:
            del all_tables[item][member]
        d = []

# porovnani vztahu a urceni vysledneho
def return_rel(A,B):
    if A==B:
        return A
    else:
        return "N:M"

# zpracovani prepinace -g
def generate_rel(all_tables):
    # pomocny slovnik pro zaznamenani vztahu
    tables_g = {}
    for item in all_tables:
        if item not in tables_g.keys():
            tables_g[item] = {}
        for i in all_tables[item]:
            if item != i:
                # zaznamename vztahy podle ulozenych FK
                if i in tables_g.keys():
                    if item not in tables_g[i].keys():
                        tables_g[item][i] = "N:1"
                        tables_g[i][item] = "1:N"
                    # tabulky se odkazuji vzajemne
                    else:
                        tables_g[item][i] = "N:M"
                        tables_g[i][item] = "N:M"
                # pro tabulku i jeste neni zaznam
                else:
                    tables_g[item][i] = "N:1"
                    tables_g[i]= {}
                    tables_g[i][item] = "1:N"
    work = True
    # je treba najit vsechny nezaznamenane vztahy
    while work:
        work = False
        # pomocny slovnik pro ulozeni vztahu
        aux={}
        for item in tables_g.keys():
            for i in tables_g[item].keys():
                for y in tables_g[i].keys():
                    # vztah uz mame zapsany
                    if y==item:
                        continue
                    # novy vztah, zaznamename
                    if y not in tables_g[item].keys():
                        work = True
                        rel = return_rel(tables_g[item][i],tables_g[i][y])
                        if not item in aux:
                            aux[item]={}
                        aux[item][y]=rel
        # ulozime vztahy z pomocneho slovniku k jiz ulozenym vztahum
        for mem in aux.keys():
            for m in aux[mem]:
                if mem in tables_g:
                    if m in tables_g[mem]:
                        tables_g[mem][m]=aux[mem][m]
                    else:
                        tables_g[mem][m]=aux[mem][m]
                else:
                    tables_g[mem]={}
                    tables_g[mem][m]=aux[mem][m]
    return tables_g

# vypis vztahu na vystup
def print_rel(params, tables_g, f):
    # je treba tisknout hlavicku
    if len(params.header) == 1:
        f.write("--"+params.header[0]+"\n\n")
    f.write("<tables>\n")
    for item in tables_g:
        f.write('   <table name="'+item+'">\n')
        f.write('      <relation to="'+item+'" relation_type="1:1" />\n')
        for i in tables_g[item]:
            f.write('      <relation to="'+i)
            f.write('" relation_type="'+tables_g[item][i]+'" />\n')
        f.write('   </table>\n')
    f.write('</tables>')

# kontrolujeme, zda muzeme vlozit data z dalsiho souboru do tabulek vytvorenych z dat stavajicich
def validation(params, all_tables, all_attrs):
    # osetreni souboru
    try:
      datain = open(params.isvalid[0])
    except IOError:
        print('Soubor k validaci nelze otevrit', file=sys.stderr)
        sys.exit(2)
    # parsujeme data v souboru
    dom = parse(datain)
    root = dom.firstChild
    aux = root
    val_tables = {}
    val_attrs = {}
    parse_xml(val_tables, val_attrs, params, root, aux)
    # neni zadano -b, pouzijeme pocitadlo
    if params.b == 0:
        set_counter(val_tables)
    # kontrola konfliktu
    if params.g == 0:
        conflict(val_tables, val_attrs)
    # porovnani, zda muzeme elementy z druhe tabulky pridat do tabulky prvni
    for item in val_tables:
        if item not in all_tables.keys():
            return False
        for mem in val_tables[item]:
            if mem not in all_tables[item]:
                return False
    # porovnani, zda muzeme atributy z druhe tabulky pridat do tabulky prvni
    for tab in val_attrs:
        if tab not in all_attrs:
            return False
        for mem in val_attrs[tab]:
            if mem not in all_attrs[tab] or all_attrs[tab][mem] < val_attrs[tab][mem]:
                return False
    return True

####MAIN####
def main():
    all_tables = {}
    all_attrs = {}
    params = parseArgs()

    # vstup
    if (len(params.infile) == 1):
        try:
            datain = open(params.infile[0],'r')
        except IOError:
            print('Soubor nelze otevrit', file=sys.stderr)
            sys.exit(2)
    else:
        datain = sys.stdin
    # vystup
    if (len(params.outfile) == 1):
        try:
            dataout = open(params.outfile[0], "w")
        except IOError:
            print('Soubor nelze otevrit', file=sys.stderr)
            sys.exit(2)
    else:
        dataout = sys.stdout
    # parsujeme vstup
    dom = parse(datain)
    root = dom.firstChild
    aux = root
    parse_xml(all_tables, all_attrs, params, root, aux)
    # je zadano --etc
    if len(params.etc) == 1:
        my_etc(int(params.etc[0]), all_tables, all_attrs)
    # je zadano -g
    if params.g == 1:
        conflict(all_tables, all_attrs)
        tables_g = generate_rel(all_tables)
        if len(params.isvalid)==1:
            if validacia(params, all_tables, all_attrs):
                print_rel(params, tables_g, dataout)
                sys.exit(0)
            else:
                print("Vkladana data nejsou validni!", file=sys.stderr)
                sys.exit(91)
        else:
            print_rel(params, tables_g, dataout)
            sys.exit(0)
    # neni zadano -b, pouzijeme pocitadlo
    if params.b == 0:
        set_counter(all_tables)
    # kontrola konfliktu v nazvech
    if params.g == 0:
        conflict(all_tables, all_attrs)
    # budeme kontrolovat validitu
    if len(params.isvalid)==1:
        if validation(params, all_tables, all_attrs):
            print_table(params, dataout, all_tables, all_attrs)
        else:
            print("Vkladana data nejsou validni!", file=sys.stderr)
            sys.exit(91)
    # vytiskneme vysledky
    else:
        print_table(params, dataout, all_tables, all_attrs)

if __name__ == "__main__":
    main()
