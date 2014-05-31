// fce getCookie a setCookie jsem prevzala ze sveho zdrojoveho kodu ze cviceni c.5
function getCookie(NameOfCookie){
  if (document.cookie.length > 0) {              
  begin = document.cookie.indexOf(NameOfCookie+"=");       
    if (begin != -1) {           
    begin += NameOfCookie.length+1;       
    end = document.cookie.indexOf(";", begin);
      if (end == -1) end = document.cookie.length;
    return unescape(document.cookie.substring(begin, end));
    }     
  }
  return -1;
}
    
function setCookie(NameOfCookie, value) {
  document.cookie = NameOfCookie + "=" + String(value);
}

function changeVisibility(NameOfCookie)
{
  var value=getCookie(NameOfCookie);
  var elm=document.getElementById(NameOfCookie);
  
  if(value == 1)
  {
    elm.style.display="none";
    setCookie(NameOfCookie,2);
  }
  else if(value == -1)
  {
    elm.style.display="none";
    setCookie(NameOfCookie,2);
  }
  else
  {
    elm.style.display="block";
    setCookie(NameOfCookie,1);
  }
}

function setVisibility()
{
   var value=getCookie("first_new");
   var elm=document.getElementById("first_new");
   if(value == -1)
   {
     setCookie("first_new",2);
     elm.style.display="none";
   }
   else
   {
     if (value == 1)
     {
       elm.style.display="block";
     }
     else
     {
       elm.style.display="none";
     }
     value=getCookie("second_new");
     elm=document.getElementById("second_new");
   }
   if(value == -1)
   {
     setCookie("second_new",2);
     elm.style.display="none";
   }
   else
   {
     if (value == 1)
     {
       elm.style.display="block";
     }
     else
     {
       elm.style.display="none";
     }
   }
}