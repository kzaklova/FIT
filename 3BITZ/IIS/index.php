<?php

if (!isset($_SESSION)) 
session_start();


include "www.php";
include "zam.php";
include "db_init.php";
include "edit.php";
session_timeout();

// hlavicka
include "menu1.html";

// prvni nacteni stranky
if (!isset($_GET["menu"])){
  $_GET["menu"]=0;
}


//login
if(isset($_POST["name"]) and isset($_POST["pass"]) and $_POST["name"]!=""){
  $query="SELECT * FROM pracovnik WHERE login='".$_POST["name"]."'";
  $result = @mysql_query($query,$db); 
  while ($data = mysql_fetch_array($result, MYSQL_ASSOC)) {
    if($data["heslo"]==$_POST["pass"]){
      switch($data["typ_zam"]){
        case 1:
          $_GET["z"]=0;
          $_SESSION['login']=1;
           unset($_GET["menu"]);
          $_SESSION['last_activity']=time();
          break;
        case 2:
          $_SESSION['login']=2;
          $_GET["z"]=0;
          unset($_GET["menu"]);
          $_SESSION['last_activity']=time();
          break;

      }
    }
  }
}

//login
if(isset($_POST["zname"]) and isset($_POST["zpass"]) ){
  $query="SELECT * FROM zakaznik WHERE login='".$_POST["zname"]."'";
  $result = @mysql_query($query,$db); 
  while ($data = mysql_fetch_array($result, MYSQL_ASSOC)) {
    if($data["heslo"]==$_POST["zpass"]){
     
          $_SESSION['log']=1;
          $_SESSION['zakaznik']=$data["id"];
         
  }
}
}




// nacteni menu uzivatele

if (!isset($_SESSION['login']) and !isset($_SESSION['log'])){
  www($_GET["menu"],$db);
}
else{
   $_SESSION['last_activity']=time();
 // zakaznik 
 if(isset($_SESSION['log'])){
 if (isset($_GET["nakup"]))
    xxx($db);
   else
    zakaznik($db);
    if(isset($_GET["menu"]))
      if ($_GET["menu"]==0)   
       unset($_SESSION['log']);
}

else{
 // zamestnanec
 if ($_GET["z"]==20){ 
    unset($_GET["z"]);
    unset($_SESSION['login']);
    session_unset();
    session_destroy();
    $_GET["menu"]=5;
    ?>
      <script>
        window.location.href="index.php";
      </script>
     <?    
  }
  else{ 
    zamestnanci($_GET["z"], $db,$_SESSION['login']);
}
  

}}


// zapati
include "menu2.html";

/*

*/

?>
