<?
$DB_HOST = "localhost:/var/run/mysql/mysql.sock";
$DB_USERNAME = "xzaklo00";
$DB_PASSWORD = "o2abetun";
$DB_NAME = "xzaklo00";

$db=mysql_connect($DB_HOST,$DB_USERNAME,$DB_PASSWORD);

if (!mysql_select_db($DB_NAME,$db)) {
	echo "chyba pøi pøipojení k databázi.";
	exit();
}
?>

