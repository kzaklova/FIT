<?


if (isset($_POST["smazat_objednavka"]))
{ 

  $query="DELETE FROM objednavka WHERE `id` = '".mysql_real_escape_string($_POST["id_upravit_objednavka"])."';";
  $result = @mysql_query($query,$db);
  if(!($result)) echo "poruseni integritniho omezenei";



}


if (isset($_POST["upravit_objednavka"])){

  $_GET["z"]=9;

}
if (isset($_POST["ulozit_objednavku"])){

  $query="UPDATE `objednavka` SET `zpusob_platby`='"
  .mysql_real_escape_string($_POST["platba"])
  ."', `zpusob_dodani`='"
  .mysql_real_escape_string($_POST["dodani"])
  ."'WHERE `id`='".mysql_real_escape_string($_SESSION["id"])
   ."';";
  $result = @mysql_query($query,$db);
  if(!($result))
    echo "Chyba: Sna¾íte se poru¹it integritní omezení.";

}



if (isset($_POST["smazat_pecivo"]))
{ 
  $query="DELETE FROM pecivo WHERE `id` = '".mysql_real_escape_string($_POST["id_upravit_pecivo"])."';";
  $result = @mysql_query($query,$db);
  if(!($result)) echo "poruseni integritniho omezenei";
  $_GET["z"]=2;


}


if (isset($_POST["smazat_pracovnik"]))
{ 

  $query="DELETE FROM pracovnik WHERE `id` = '".mysql_real_escape_string($_POST["id_upravit_pracovnik"])."';";
  $result = @mysql_query($query,$db);
  if(!($result)) echo "poruseni integritniho omezenei";
  $_GET["z"]=5;


}


if (isset($_POST["smazat_oblast"]))
{ 

  $query="DELETE FROM oblast WHERE `id` = '".mysql_real_escape_string($_POST["id_upravit_oblast"])."';";
  $result = @mysql_query($query,$db);
  if(!($result)) echo "poruseni integritniho omezenei";



}

if (isset($_POST["pridat_oblast"]))
{ 

  
 

  $query="INSERT INTO oblast (`jmeno`) VALUES ('"
    .mysql_real_escape_string($_POST["nazev_oblast"])."');";
  $result = @mysql_query($query,$db);
  if(!($result))
    echo "Chyba: Sna¾íte se poru¹it integritní omezení.";



}

if (isset($_POST["vybrat_id_ridic"])){

$_GET["z"]=4;

}


if (isset($_POST["vybrat_smaz_id_ridic"])){

$_GET["z"]=7;

}



if (isset($_POST["priradit_oblast_id"])){

  $query="UPDATE `oblast` SET `id_pracovnik`='"
    .mysql_real_escape_string($_SESSION["id"]).
    "' WHERE `id`='".mysql_real_escape_string($_POST["oblast_id"]).
    "';";
  $result = @mysql_query($query,$db);
  if(!($result))
   echo  "Chyba: Sna¾íte se poru¹it integritní omezení.";

}

if (isset($_POST["odebrat_oblast_id"])){

  $query="UPDATE `oblast` SET `id_pracovnik`=NULL WHERE `id`='".mysql_real_escape_string($_POST["oblast_id"]).
    "';";
  $result = @mysql_query($query,$db);
  if(!($result))
    echo  "Chyba: Sna¾íte se poru¹it integritní omezení.";

}


if (isset($_POST["pridat_produkt"]))
{ 

  
  if (is_numeric($_POST["cena"])){

  $query="INSERT INTO pecivo (`nazev`,`cena`) VALUES ('"
    .mysql_real_escape_string($_POST["nazev"])."','"
    .mysql_real_escape_string($_POST["cena"])."');";
  $result = @mysql_query($query,$db);
  if(!($result))
    echo "Chyba: Sna¾íte se poru¹it integritní omezení.";
  $_GET["z"]=2;
  unset($_SESSION["nazev"]);
}
else {
$_SESSION["nazev"]=$_POST["nazev"];

}}


if(isset($_POST["upravit_pecivo"])){
$_SESSION["id"]=$_POST["id_upravit_pecivo"];
$_GET["z"]=10;

}

if(isset($_POST["upravit_produkt"])){
if (is_numeric($_POST["cena"])){

  $query="UPDATE pecivo SET `nazev`='"
    .mysql_real_escape_string($_POST["nazev"]).
    "',`cena`='"
    .mysql_real_escape_string($_POST["cena"])
    ."'WHERE `id`='".mysql_real_escape_string($_SESSION["id"])
    ."';";
  $result = @mysql_query($query,$db);
  if(!($result))
    echo "Chyba: Sna¾íte se poru¹it integritní omezení.";
  $_GET["z"]=2;
  unset($_SESSION["nazev"]);
}
else {
  $_SESSION["nazev"]=$_POST["nazev"];
  $_GET["z"]=10;
}}

if(isset($_POST["pridat_zamestnance"])){

$pozice=$_POST["pozice"]+1;
$er=0;


/*
if(preg_match("~^[a-zA-Z]$+~", $_POST["jmeno"])) {
$_SESSION["jmeno"]=$_POST["jmeno"];
}else
{echo "jmeno";
$er=1;}



/*
if(eregi("^[a-zA-Z]+", $_POST["prijmeni"])) {
$_SESSION["prijmeni"]=$_POST["prijmeni"];
}else{
$er=1;
echo "prijmeni";}
if(eregi("^[1-9]{10}[/][0-9]{4}", $_POST["cbu"])) {
$_SESSION["cbu"]=$_POST["cbu"];
}else{
echo"cbu";
$er=1;
}


 */

if($er==0){

 $query="INSERT INTO pracovnik (`jmeno`,`prijmeni`,`bydliste`,`cislo_bu`,`typ_zam`,`heslo`,`login`) VALUES ('"
    .mysql_real_escape_string($_POST["jmeno"])."','"
    .mysql_real_escape_string($_POST["prijmeni"])."','"
    .mysql_real_escape_string($_POST["bydliste"])."','"
    .mysql_real_escape_string($_POST["cbu"])."','"
    .mysql_real_escape_string($pozice)."','"
    .mysql_real_escape_string($_POST["heslo"])."','"
    .mysql_real_escape_string($_POST["login"])."');";
  

$result = @mysql_query($query,$db);
  if(!($result))
    echo "Chyba: Sna¾íte se poru¹it integritní omezení.";

}


$_GET["z"]=5;
}


if(isset($_POST["upravit_pracovnik"])){
 $_GET["z"]=11;
 $_SESSION["id"]=$_POST["id_upravit_pracovnik"];



}

if(isset($_POST["ulozit_zakaznik"])){
echo("here");
echo $_POST["jmeno"];
echo $_POST["login"];
echo $_POST["heslo"];
echo $_POST["prijmeni"];
echo $_POST["adresa"];

$query="INSERT INTO zakaznik (`jmeno`,`login`,`heslo`,`prijmeni`,`adresa`) VALUES ('"
 .mysql_real_escape_string($_POST["jmeno"])."','"
  .mysql_real_escape_string($_POST["login"])."','"
  .mysql_real_escape_string($_POST["heslo"])."','"
   .mysql_real_escape_string($_POST["prijmeni"])."','"
   .mysql_real_escape_string($_POST["adresa"])."');";
 

 $result = @mysql_query($query,$db);
  if(!($result))
    echo "Chyba: Sna¾íte se poru¹it integritní omezení.";
  
$_GET["menu"]=2;
  
}


if(isset($_POST["zacit_nakupovat"])){


$query="INSERT INTO objednavka (`zpusob_platby`,`datum`,`zpusob_dodani`,`id_zakaznik`) VALUES ('"
 .mysql_real_escape_string($_POST["platba"])."','now','"
   .mysql_real_escape_string($_POST["dodani"])."','"
   .mysql_real_escape_string($_SESSION["zakaznik"])."');";

 $result = @mysql_query($query,$db);
  if(!($result))
    echo "Chyba: Sna¾íte se poru¹it integritní omezení.";
$_GET["nakup"]=1;





}

?>


