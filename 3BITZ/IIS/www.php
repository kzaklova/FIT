<?

function xxx($db){
?>
<!---menu--->
	<div class="menu">
		<ul>
			<li><a href="?menu=0">odhlásit</a></li>
			
		</ul>
	</div>
<!---content--->
	<div class="center_content">


<?
$query="SELECT * FROM pecivo";
$result = @mysql_query($query,$db);

?>

<!---left content--->
		<div class="center_left">
			
<? while ($data = mysql_fetch_array($result, MYSQL_ASSOC)) {
?>
<form method="POST" >
<input type="hidden" name="pridat_info" value="<?php echo $data["id"]?>">
<div class="shop">
<div class="shop_title"><?php echo $data["nazev"]?></div>
				<div class="shop_body"><?php echo $data["cena"]?>Kè<br><input type="text" value="1" name="pocet" class="shop_ks">ks<input type="submit" value="do ko¹íku"></div>
			</div>
</form>

<?}?>

	
			<input type="submit" value="objednat" class="cudlik">
			</form>
				
				
		</div> 



<!---right content--->
		<div class="center_right">
			
			<div class="image_box"><img src="images/chleba.jpg" alt="" title="" width="220px"/></div>
		</div>
<!---wait for content--->
		<div class="clear"></div> 
	</div>
<!---footer--->
	<div id="footer">
		<div class="left_footer"></div>

<?

}?>



<?

function zakaznik($db){
?>
<!---menu--->
	<div class="menu">
		<ul>
			<li><a href="?menu=0">odhlásit</a></li>
			
		</ul>
	</div>
<!---content--->
	<div class="center_content">




<?
  
$zpusob_dodani = array(
        "rozvoz",
        "osobne" );
      $zpusob_platby = array(
        "hotovost",
        "kartou" 
      );


?>
<!---left content--->
		<div class="center_left">
			<form method="POST">
				<table>
				
				<tr><td>Zpùsob platby:</td><td>   
<?   
echo "  <select name='platba'>\n";
foreach ($zpusob_platby as $key => $druh) {
	echo "<option value='$key'";
        echo">$druh</option>\n";

}
echo "</select>\n";
?>
</td></tr>				
				
<tr><td>Zpùsob dodání:</td><td>   
<?   
echo "  <select name='dodani'>\n";
foreach ($zpusob_dodani as $key => $druh) {
	echo "<option value='$key'";
        echo">$druh</option>\n";

}
echo "</select>\n";
?>
</td></tr>


				<tr><td><input type="submit" name="zacit_nakupovat" value="zaèít nakupovat"></td><td></td></tr>
				</table>
			</form>
		</div> 
<!---right content--->
		<div class="center_right">
			
			<div class="image_box"><img src="images/chleba.jpg" alt="" title="" width="220px"/></div>
		</div>

<!---wait for content--->
		<div class="clear"></div> 
	</div>
<!---footer--->
	<div id="footer">
		<div class="left_footer"></div>

<?

}?>

<?
function www($num,$db){
/// menu
?>

<!---menu--->
	<div class="menu">
		<ul>
			<li <?if($num==0){?>class="selected"<?}?>><a href="?menu=0">home</a></li>
			<li <?if($num==1){?>class="selected"<?}?>><a href="?menu=1">o nás</a></li>
			<li <?if($num==2){?>class="selected"<?}?>><a href="?menu=2">produkty</a></li>
			<li <?if($num==3){?>class="selected"<?}?>><a href="?menu=3">dovoz</a></li>
			<li <?if($num==4){?>class="selected"<?}?>><a href="?menu=4">kontakt</a></li>
		</ul>
	</div>
<!---content--->
	<div class="center_content">

<?
////////////////////////////////////////////////
//// obsah stranek
///////////////////////////////////////////
switch($num){
case 0:
?>
<!---left content--->
		<div class="center_left">
			<div class="welcome_box">
				<p class="welcome"><span class="orange">Vítejte v pekárnì U Dvou volù,</span><br />
					jsme rodinný podnik s dlouholetou tradicí. Nabízíme velkou ¹kálu produktù od rohlíkù pøes chleby a¾ po buchty. Produkty je mo¾né odebírat na na¹em výdejním místì v Brnì nebo je zde mo¾nost dovozu do Va¹í destinace.
				</p>
			</div>
		</div> 
<!---right content--->
		<div class="center_right">
			<div class="image_box"><img src="images/chleba.jpg" alt="" title="" width="220px"/></div>
		</div>
<? break;
case 1:
?>

<!---left content--->
		<div class="center_left">
			<div class="welcome_box">
				<p class="welcome">
					Jsme firma s dlouholetou tradicí. Nabízíme rohlíky, chleba, koblihy, buchty a vánoèky. Produkty je mo¾né odebírat pøímo na výdejním místì nebo je zde mo¾nost pravidelných rozvozù a¾ do Va¹í provozovny. Tady by je¹tì mohla být spousta textu o nás, ale u¾ nevím, co tu mám vymý¹let, tak¾e to asi bude v¹echno. :-)
				</p>
			</div>
		</div> 
<!---right content--->
		<div class="center_right">
			<div class="image_box"><img src="images/chleba.jpg" alt="" title="" width="220px"/></div>
		</div>
<?break;
case 2:

$query="SELECT * FROM pecivo";
$result = @mysql_query($query,$db);

?>

<!---left content--->
		<div class="center_left">
			
<? while ($data = mysql_fetch_array($result, MYSQL_ASSOC)) {
?>
<form method="POST" >
<input type="hidden" name="pridat_info" value="<?php echo $data["id"]?>">
<div class="shop">
<div class="shop_title"><?php echo $data["nazev"]?></div>
				<div class="shop_body"><?php echo $data["cena"]?>Kè<br></div>
			</div>
</form>

<?}?>



				
				
		</div> 
<!---right content--->
		<div class="center_right">
			<ul class="list">
				<li><span></span><a href="?menu=7">pøihlásit</a></li>
                                <li><span></span><a href="?menu=6">registrovat</a></li>
				
			</ul>
			<div class="image_box"><img src="images/chleba.jpg" alt="" title="" width="220px"/></div>
		</div>
   <?break;
case 3:

?>

<!---left content--->
		<div class="center_left">
			<div class="welcome_box">
				<p class="welcome"><span class="orange">Nabízíme pravidelný rozvoz do nìkolika rùzných oblastí:</span><br />
				<ul>


<?
//////////////////////////////

$query="SELECT * FROM oblast";
$result = @mysql_query($query,$db);
while ($data = mysql_fetch_array($result, MYSQL_ASSOC)) {
?>
<li><?echo$data["jmeno"]; ?>

<?}
?>
				</ul>
				</p>
			</div>
		</div> 
<!---right content--->
		<div class="center_right">
			<div class="image_box"><img src="images/chleba.jpg" alt="" title="" width="220px"/></div>
		</div>
<?break;
case 4:

?>

<!---left content--->
		<div class="center_left">
			<div class="welcome_box">
				<p class="welcome">
					Bo¾etìchova 2 <br>
					Brno, Královo pole <br><br>
					telefon: 777 123 456 <br>
					email: <a href="mailto:xzaklo00@stud.fit.vutbr.cz">xzaklo00@stud.fit.vutbr.cz</a>
				</p>
			</div>
		</div> 
<!---right content--->
		<div class="center_right">
			<div class="image_box"><img src="images/chleba.jpg" alt="" title="" width="220px"/></div>
		</div>

<?break;
case 5:
?>

<!---left content--->
		<div class="center_left">
			<form method="POST"><div class="text_box">
				<div class="title">Pøihlá¹ení</div>
				<div class="login_form_row"><label class="login_label">Jméno:</label><input type="text" name="name" class="login_input" /></div>
				<div class="login_form_row"><label class="login_label">Heslo:</label><input type="password" name="pass" class="login_input" /></div>
				<input type="submit" value="login" class="cudlik" />
			</div></form>
		</div> 
<!---right content--->
		<div class="center_right">
			<div class="image_box"><img src="images/chleba.jpg" alt="" title="" width="220px"/></div>
		</div>




<?break;
case 6:
  

?>
<!---left content--->
		<div class="center_left">
			<form method="POST">
				<table>
				<tr><td>Jméno:</td><td><input type="text" name="jmeno" ></td></tr>
				<tr><td>Pøíjmení:</td><td><input type="text" name="prijmeni"></td></tr>
				<tr><td>Adresa:</td><td><input type="text" name="adresa"></td></tr>
                                <tr><td>Login:</td><td><input type="text" name="login"></td></tr>
                                <tr><td>Heslo:</td><td><input type="text" name="heslo"></td></tr>
				
			


				<tr><td><input type="submit" name="ulozit_zakaznik" value="ulo¾it"></td><td></td></tr>
				</table>
			</form>
		</div> 
<!---right content--->
		<div class="center_right">
			
			<div class="image_box"><img src="images/chleba.jpg" alt="" title="" width="220px"/></div>
		</div>


<?break;
case 7:
?>

<!---left content--->
		<div class="center_left">
			<form method="POST"><div class="text_box">
				<div class="title">Pøihlá¹ení</div>
				<div class="login_form_row"><label class="login_label">login:</label><input type="text" name="zname" class="login_input" /></div>
				<div class="login_form_row"><label class="login_label">Heslo:</label><input type="password" name="zpass" class="login_input" /></div>
				<input type="submit" value="login" class="cudlik" />
			</div></form>
		</div> 
<!---right content--->
		<div class="center_right">
			<div class="image_box"><img src="images/chleba.jpg" alt="" title="" width="220px"/></div>
		</div>



<?
}//////////////////////
/////zapati
/////////////////////
?>
<!---wait for content--->
		<div class="clear"></div> 
	</div>
<!---footer--->
	<div id="footer">
		<div class="left_footer"><a href="?menu=0">home</a> <a href="?menu=5">zamìstnanec</a><a href="?menu=4">kontakt</a></div>
<?

}?>
