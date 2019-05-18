const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<style>
.card{
    max-width: 400px;
     min-height: 250px;
     background: #02b875;
     padding: 30px;
     box-sizing: border-box;
     color: #FFF;
     margin:20px;
     box-shadow: 0px 2px 18px -4px rgba(0,0,0,0.75);
}
</style>
<body>
 
<div class="card">
  <h4>Smart Stock</h4><br>
  <h1>STATUS:<span id="RFIDid">0</span></h1><br>
</div>
<script>
 
setInterval(function() {
  // Call a function repetatively with 100m Second interval
  getData();
}, 100); //20mSeconds update rate
 
function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("RFIDid").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "readrfid", true);
  xhttp.send();
}
</script>
</body>
</html>
)=====";
