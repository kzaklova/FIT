#!/usr/bin/perl

#JSN:xzaklo00

use strict;
use warnings;
use Data::Types qw{:all};
use Getopt::Long;
use IO::File;
use utf8;
use Encode qw(encode_utf8);
use JSON::XS;
use POSIX;
use XML::Writer;

# definice konstant
use constant {
EOK => 0,
EOPTS => 1,
EIN => 2,
EOUT => 3,
EINFORMAT => 4,
ESYM => 50,
EELEM => 51,
XML_NAMECHAR => ':A-Z_a-z\xC0-\xD6\xD8-\xF6\x{00F8}-\x{02FF}\x{0370}-\x{037D}\x{037F}-\x{1FFF}\x{200C}-\x{200D}\x{2070}-\x{218F}\x{2C00}-\x{2FEF}\x{3001}-\x{D7FF}\x{F900}-\x{FDCF}\x{FDF0}-\x{FFFD}\x{10000}-\x{EFFFF}\-\.0-9\x{00B7}\x{0300}-\x{036F}\x{203F}-\x{2040}',
XML_NAMESCHAR => ':A-Z_a-z\xC0-\xD6\xD8-\xF6\x{00F8}-\x{02FF}\x{0370}-\x{037D}\x{037F}-\x{1FFF}\x{200C}-\x{200D}\x{2070}-\x{218F}\x{2C00}-\x{2FEF}\x{3001}-\x{D7FF}\x{F900}-\x{FDCF}\x{FDF0}-\x{FFFD}\x{10000}-\x{EFFFF}'
};

# definice promennych
my $help = 0;
my @inputfile;
my @outputfile;
my @substr;
my $nohead = 0;
my @rootelem;
my @arrname;
my $arrnamewithsize;
my @itemname;
my $strelem = 0;
my $numelem = 0;
my $litattr = 0;
my $convert = 0;
my $arrsize = 0;
my $index = 0;
my @start;
my $my_file = 0;
my $my_scalar;
my $writer;
my $last = 1;


# zpracovani parametru
GetParams();

if ($help == 1) {
	PrintHelpmsg();
	exit EOK;
}

# vstupni a vystupni soubor
ReadAndWrite();

# pridani hlavicky s danym kodovanim
if (!$nohead){
	$writer->xmlDecl("UTF-8");
}

# zapis root elementu
if (scalar @rootelem > 0) {
	$writer->startTag($rootelem[0]);
}

# prevod samotny
ConvertJsnXml($my_scalar);


if (scalar @rootelem > 0) {
	$writer->endTag($rootelem[0]);
}

# ukonceni writeru
$writer->end();

# uzavreni souboru
$outputfile[1]->close() if ($my_file);

exit EOK;



# funkce pro vstupni a vystupni soubor
sub ReadAndWrite {
	local $/=undef;

	# vstup
	if (scalar @inputfile > 0) {
	# pokud mame soubor
		if (-e $inputfile[0]) {
			open (FILE, $inputfile[0]) or EndError("Nelze cist vstup\n",EIN);
			# dekodovani vstupu
			if (! eval { $my_scalar = JSON::XS->new->utf8->decode(<FILE>); 1 }) {
				EndError("Spatny format vstupu\n",EINFORMAT);
			}
			binmode FILE, ":utf8";
			close(FILE);
		}
		else {
		EndError("Vstupni soubor neexistuje\n",EIN);
		}
	}
	# neni soubor, cteme STDIN
	else {
		# dekodovani vstupu
		if (eval { $my_scalar = JSON::XS->new->utf8->decode(<STDIN>); 1 }) {
		}
		else {
			EndError("Nelze zapsat na vystup\n",EOUT);
		}
	}

	# vystup
	# pokud mame soubor
	if (scalar @outputfile > 0) {
		$my_file=1;
		$outputfile[1] = new IO::File(">$outputfile[0]");
	}
	# nemame soubor, vypisujeme na STDOUT
	else {
		open ($outputfile[1], ">&STDOUT");
	}

	$writer = XML::Writer->new( UNSAFE => 1, NEWLINES =>0, NAMESPACES => 1, DATA_MODE => 1, DATA_INDENT => 2, OUTPUT => $outputfile[1]);

	return EOK;
}

# funkce pro prevod JSN -> XML
sub ConvertJsnXml {
my $last_backup;
	# prebrani parametru
	my ($aux_scalar) = @_;
	# array
	if (ref $aux_scalar eq 'ARRAY') {
	$last_backup = $last; $last = 0;
		if ($arrsize == 0){
			$writer->startTag($arrname[0]);
		}
		# byl zadan parametr array size
		else {
			# vypisujeme velikost pole
			$arrnamewithsize=$arrname[0];
			$arrnamewithsize.=" size=\"";
			# spocitame velikost pole
			SetArrSize($aux_scalar);
			$arrnamewithsize.=$arrsize;
			$arrnamewithsize.="\"";
			# zapiseme pocatecni tag
			$writer->startTag($arrnamewithsize);
		}
		my $position;
		# je pozadovano indexovani
		if ($index) {
			# urcime pocatecni index
			$position=$start[0];
		}
		foreach my $forvar(@$aux_scalar) {
			# prvek je boolean nebo hodnota
			if ((ref $forvar eq 'JSON::XS::Boolean')||(ref $forvar eq '')) {
				# je pozadovano indexovani
				if ($index) {
					# prvek a jeho index
					$itemname[0] = $itemname[1];
					#$itemname[0]=~ s/[^ a-zA-Z0-9_.-]/-/g;
					$itemname[0].= " index=\"";
					$itemname[0].= $position;
					$position = $position + 1;
					$itemname[0].= "\"";
				}
				# prevod prvek+hodnota
				AttrValue($itemname[0], $forvar);   
			}
			# prvek je string
			else {
				# je vyzadovano indexovani
				if ($index) {
					# prvek a jeho index
					$itemname[0] = $itemname[1];
					$itemname[0]=~ s/[^ a-zA-Z0-9_.-]/-/g;
					$itemname[0].= " index=\"";
					$itemname[0].= $position;
					$position = $position + 1;
					$itemname[0].= "\"";
				}
				# zapiseme
				$writer->startTag($itemname[0]);
				ConvertJsnXml($forvar);
				$writer->endTag($itemname[1]);
			}
		}
		$writer->endTag($arrname[0]);
		$last = $last_backup;
		return EOK;
	}

	(ref $aux_scalar eq 'HASH')?($last_backup=$last,$last=1):();
	# prvek je hash nebo array
	while ( my($key, $value) = each (%$aux_scalar)) {
		# prvek je hash
		if (ref $value eq 'HASH') {
			# nahradime nevalidni znaky
			unless (scalar @substr) {
				$key=~ s/[^${\( XML_NAMECHAR)}]/-/g;
			}
			else {
				$key=~ s/[^${\( XML_NAMECHAR)}]/$substr[0]/g;
			}
			# zapiseme
		$last_backup = $last; $last=1;
			$writer->startTag($key);
			ConvertJsnXml($value);
			$writer->endTag($key);
			$last = $last_backup;
		}
		# prvek je array
		elsif (ref $value eq 'ARRAY') { 
			# nahradime nevalidni znaky
			unless (scalar @substr) {
				$key=~ s/[^${\( XML_NAMECHAR)}]/-/g;
			}
			else {
				$key=~ s/[^${\( XML_NAMECHAR)}]/$substr[0]/g;
			}
			# zapiseme
		$last_backup = $last; $last = 0;  
			$writer->startTag($key);
			ConvertJsnXml($value);
			$writer->endTag($key);
			$last = $last_backup;
		}
		# prvek ma hodnotu
		else {
			AttrValue($key,$value);
		}
	}
	$last = $last_backup;
}

# funkce pro urceni velikosti pole
sub SetArrSize{
	my ($aux_arr) = @_;
	$arrsize=0;
	foreach(@$aux_arr) {
		$arrsize += 1;
	}
}

# funkce pro prevod jmeno+hodnota
sub AttrValue{
	my $key = shift @_;
	my $value = shift @_; 
	my $myemptyTag;

	# jmeno nesmi zacinat . - :
	if (($key =~ /^\./)||($key =~ /^\-/)||($key =~ /^\:/)) {
		if ($my_file) {
			$writer->end();
			$outputfile[1]->close();
			open(DATA, ">$outputfile[0]");
			close DATA;
		}
		EndError("Nevalidni nazev elementu\n",EELEM);
	}

	# hodnota je boolean nebo null
	if ((! defined  $value)||(ref $value eq 'JSON::XS::Boolean')) {
		# neni zadano -l
		unless ($litattr) {
			if (! defined  $value) {
				$writer->emptyTag($key,'value' => 'null');
			}
			elsif ($value) {
				$writer->emptyTag($key, 'value' => 'true');
			}
			else {
				$writer->emptyTag($key, 'value' => 'false');
			}
		}
		# je zadano -l
		else {
			$writer->startTag($key);
			if (! defined  $value) {
				$writer->emptyTag('null');
			}
			elsif ($value) {
				$writer->emptyTag('true');
			}
			else {
				$writer->emptyTag('false');
			}
			$writer->endTag((($index)&&(!$last))?$itemname[1]:$key);
		}
	}
	else {
		# cislo
		if (!(encode_json( [$value]) =~ /^\[\"/) && ref($value) eq '' && defined $value) {
			# desetinne cislo
			if ( encode_json( [$value] ) =~ /^\[[-]*[0-9]*\.[0-9]*\]/ ) {
				#zaokrouhleni
				$value = floor($value);
			}
			# neni zadano -i
			unless($numelem) {
				$writer->emptyTag($key, 'value' => $value);
  			}
  			else {
  				$writer->startTag($key);
  				$writer->characters($value);
  				$writer->endTag((($index)&&(!$last))?$itemname[1]:$key);
  			}
  		}
  		#string
  		else {
			# neni zadano -c
			unless ($convert) {
			unless ($index){
				# neni zadano -h
				unless (scalar @substr) {
					$key =~ s/[^${\( XML_NAMECHAR)}]/-/g
				}
				else {
					$key=~ s/[^${\( XML_NAMECHAR)}]/$substr[0]/g;
				}
			}
			}
			else {
				$value = InvalidCharacters($value);
			}
			# jmeno nezacina . - :
			if (($key =~ /^\./)||($key =~ /^\-/)||($key =~ /^\:/)) {
				if ($my_file) {
					$writer->end();
					$outputfile[1]->close();
					open(DATA, ">$outputfile[0]");
					close DATA;
				}
				EndError("Nevalidni nazev elementu\n",EELEM);
			}
			# neni zadano -s
			unless($strelem) {
				$writer->raw("\n"); 
				$myemptyTag = "<";
				$myemptyTag .= $key;
				$myemptyTag .= " value=\"";
				$myemptyTag .= $value;
				$myemptyTag .= "\"/>";   
				$writer->raw($myemptyTag);
			}
			else {
				$writer->startTag($key);
				$writer->raw($value);
				$writer->endTag((($index)&&(!$last))?$itemname[1]:$key);
			}
		}
	}
}

# funkce pro konvertovani znaku ' " < > &
sub InvalidCharacters{
	my ($conv) = @_;
	$conv =~ s/&qt;/pseudorandomstringxxx1/g;
	$conv =~ s/&lt;/pseudorandomstringxxx2/g;
	$conv =~ s/&quot;/pseudorandomstringxxx3/g;
	$conv =~ s/&apos;/pseudorandomstringxxx4/g;
	$conv =~ s/&amp;/pseudorandomstringxxx5/g;
	$conv=~ s/&/&amp;/g; 
	$conv=~ s/[\']/&apos;/g;
	$conv=~ s/[\"]/&quot;/g;
	$conv=~ s/[<]/&lt;/g;
	$conv=~ s/[>]/&gt;/g;
	$conv =~ s/pseudorandomstringxxx1/&qt;/g;
	$conv =~ s/pseudorandomstringxxx2/&lt;/g;
	$conv =~ s/pseudorandomstringxxx3/&quot;/g;
	$conv =~ s/pseudorandomstringxxx4/&apos;/g;
	$conv =~ s/pseudorandomstringxxx5/&amp;/g;

	return $conv;
}

# funkce pro zpracovani parametru
sub GetParams{
	GetOptions(
		"help+" => \$help,
		"input=s" => \@inputfile,
		"output=s" => \@outputfile,
		"h=s" => \@substr,
		"n+" => \$nohead,
		"r=s" => \@rootelem,
		"array-name=s" => \@arrname,
		"item-name=s" => \@itemname,
		"s+" => \$strelem,
		"i+" => \$numelem,
		"l+" => \$litattr,
		"c+" => \$convert,
		"a+" => \$arrsize,
		"array-size+" => \$arrsize,
		"t+" => \$index,
		"index-items+" => \$index,
		"start=i" => \@start
	) or EndError("Chybne zadane parametry\n",EOPTS);

	# help + jiny argument
	if ( ($help == 1)&&( (scalar @inputfile > 0)||(scalar @outputfile > 0)||(scalar @substr > 0)||($nohead > 0)||(scalar @rootelem > 0)||(scalar @arrname > 0)||(scalar @itemname > 0)||($strelem > 0)||($numelem > 0)||($litattr > 0)||($convert > 0)||($arrsize > 0)||($index > 0)||(scalar @start > 0) ) ) {
		EndError("Napovedu zobrazite vypsanim --help\n",EOPTS);
	}

	# vicekrat pouzity argument
	if ( ($help > 1)||($nohead > 1)||($strelem > 1)||($numelem > 1)||($litattr > 1)||($convert > 1)||($arrsize > 1)||($index > 1) ) {
		EndError("Opakovany parametr\n",EOPTS);
	}
	if ( (scalar @inputfile > 1)||(scalar @outputfile > 1)||(scalar @substr > 1)||(scalar @rootelem > 1)||(scalar @arrname > 1)||(scalar @itemname > 1)||(scalar @start > 1) ) {
		EndError("Opakovany parametr\n",EOPTS);
	}

	# urceni implicitniho podretezce pro nahrazeni
	if (scalar @substr == 0) {
		$substr[0] = '-';
	}
	utf8::decode($substr[0]);

	# index pole < 0
	if (scalar @start > 0) {
		if ($start[0] < 0) {
			EndError("Index pole musi byt kladne cislo\n",EOPTS);
		}
	}

	# validni root element
	if (scalar @rootelem == 1) {
		if ($rootelem[0] =~ /[<&>\"\']/) {
			EndError("Nevalidni XML znak\n",ESYM);
		}
		utf8::decode($rootelem[0]);
	}

	# implicitni urceni nazvu pole
	if (scalar @arrname == 0) {
		$arrname[0] = 'array';
	}

	utf8::decode($arrname[0]);

	# validni nazev pole
	if (scalar @arrname == 1) {
		if ($arrname[0] =~ /[<&>\"\']/) {
			EndError("Nevalidni XML znak\n",ESYM);
		}
	}

	# implicitni urceni nazvu prvku
	if (scalar @itemname == 0) {
		$itemname[0] = 'item';
	}
	utf8::decode($itemname[0]);

	# validni nazev prvku
	if (scalar @itemname == 1) {
		if ($itemname[0] =~ /[<&>\"\']/) 
		{
			EndError("Nevalidni XML znak\n",ESYM);
		}
						# neni zadano -h
				unless (scalar @substr) {
					$itemname[0] =~ s/[^${\( XML_NAMECHAR)}]/-/g
				}
				else {
					$itemname[0]=~ s/[^${\( XML_NAMECHAR)}]/$substr[0]/g;
				}
		$itemname[1] = $itemname[0];
	}

	# --start bez --index-items
	if ( (scalar @start > 0)&&($index == 0) ) {
		EndError("--start musi byt pouzit s --index-items\n",EOPTS);
	}

	# validni pozice --start
	if (scalar @start == 1) {
		if ($start[0] =~ /[^[0-9]*]/) {
			EndError("--start musi byt kladne cislo\n",EOPTS);
		}
	}

	# implicitni urceni hodnoty start
	if (scalar @start == 0) {
		$start[0] = 1;
	}

	return EOK;
}

sub PrintHelpmsg {
print("IPP - JSON to XML\n");
print("--help                      vypise napovedu\n");
print("--input=filename            vstupni JSN soubor ve formatu UTF-8, pokud\n");
print("                            neni zadan, skript cte standardni vstup\n");
print("--output=filename           vystupni XML soubor ve formatu UTF-8, pokud\n");
print("                            neni zadan, skript vypisuje na standardni vystup\n");
print("-h=subst                    ve jmene elementu odvozeneho z dvojice\n");
print("                            jmeno-hodnota nahradi kazdy nepovoleny XML znak\n");
print("                            znakem substr, implicitne nahrazuje znakem -,\n");
print("                            nevalidni nahrazeni vede k chybe\n");
print("-n                          negeneruje XML hlavicku na vystup\n");
print("-r=root-elem                jmeno paroveho korenoveho elementu obalujici\n");
print("                            vysledek, pokud neni zadan, neobaluje vysledek,\n");
print("                            nevalidni root-element vede k chybe\n");
print("--array-name=array-element  umozni prejmenovat element obalujici pole\n");
print("                            z array na array-element, nevalidni\n");
print("                            array-element vede k chybe\n");
print("--item-name=item-element    meni jmeno elementu z item na item-element,\n");
print("                            nevalidni item-element vede k chybe\n");
print("-s                          transformuje hodnoty typu string na elementy\n");
print("                            misto atributu\n");
print("-i                          transformuje hodnoty typu number na elementy\n");
print("                            misto atributu\n");
print("-l                          transformuje hodnoty literalu (true, false,\n");
print("                            null) na elementy misto atributu\n");
print("-c                          aktivuje preklad problematickych znaku\n");
print("                            na zapisy v XML\n");
print("-a, --array-size            doplni u pole atribut size s poctem prvku\n");
print("                            v poli\n");
print("-t, --index-items           ke kazdemu prvku v poli prida atribut index\n");
print("                            s pozici prvku v poli\n");
print("--start=n                   inicializuje citac pro indexaci prvku pole\n");
print("                            na zadane cele cislo (0-inf), implicitne 1,\n");
print("                            nutno kombinovat s -t\n");
}

# funkce pro vypis chybove hlasky a skonceni s chybou
sub EndError {
	my ($err_message, $err_number) = @_;
	if (($err_number == EELEM)||($err_number == ESYM)) {
		if (scalar @outputfile > 0) {
			$my_file=1;
			$outputfile[1] = new IO::File(">$outputfile[0]");
			$outputfile[1]->close();
		}
	}
	print STDERR $err_message;
	exit $err_number;
}
