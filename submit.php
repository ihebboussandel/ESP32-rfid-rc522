<?php

define("host",'localhost');
define('username','root');
define('pwd','');
define('db','rfid');
define('table', 'tag');
$time=date('Y-m-d H:i:s');
$number="";
function article_sequence_ajoute($conn) {
            $sql="UPDATE `articles` SET `quantite`=`quantite`+1";

			if ($conn->query($sql) === TRUE) {
				echo "article entré ";
			} else {
				echo "Error updating record: " . $conn->error;
			}

         }
function article_sequence_suprime($conn) {
            $sql="UPDATE `articles` SET `quantite`=`quantite`-1";

			if ($conn->query($sql) === TRUE) {
				echo "article sortie";
			} else {
				echo "Error updating record: " . $conn->error;
			}

		}
function agent_sequence($conn,$tag,$time) {
	$lock=false;
$sql="SELECT `id`, `username`, `password`, `email`, `sexe`, `tag_id`, `date_entree`, `date_sortie`, `status` FROM `users` WHERE `tag_id`='$tag'";
$result = $conn->query($sql);

if ($result->num_rows ==1) {
    // output data of each row
    while($row = $result->fetch_assoc()) {
        //echo "type: " . $row["type"]. " - Name: " . $row["firstname"]. " " . $row["lastname"]. "<br>";
		//echo " welcome :" . $row["date_entree"];
		if($row["status"]=='0'){
			
			$sql="UPDATE `users` SET `date_entree`='$time',`status`=1 where `tag_id`='$tag'";
			if ($conn->query($sql) === TRUE) {
				echo "welcome ".$row['username'] ;
				$lock=true;
			
			} else {
				echo "Error updating record(user agent): " . $conn->error;
			}
		}elseif($row["status"]=='1' ){
			
			$sql="UPDATE `users` SET `date_sortie`='$time',`status`=0 where `tag_id`='$tag'";
			if ($conn->query($sql) === TRUE) {
				echo "goodbye ". $row['username'] ;
				
			} else {
				echo "Error updating record(user agent): " . $conn->error;
			}
		}
		
    }
} else {
    //echo "0 results";
}
if($_POST['auth']=="yes" and $lock=true){
article_sequence_ajoute($conn);
}
}



if (isset($_POST["submit"])){
$conn = new mysqli(host,username,pwd,db);

if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
} 




$number=$_POST['number'];
//$sql = "INSERT INTO `rfidcard`(`tag`, `Date`) VALUES ('$number','$time')";
$sql="SELECT `type` FROM `rfidcard` WHERE `tag`='$number'";

//INSERT INTO `rfidcard`(`tag`, `Date`) VALUES ('21654654','0000-00-00 00:00:00','3ezdin',false)

//SELECT `nom`, `type`, `rf_tag` FROM `articles` WHERE `rf_tag`='1B975E01'

//SELECT `tag`, `date_entree`, `agent`, `type`, `date_sortie` FROM `rfidcard` WHERE `tag`='B9603163'
$result = $conn->query($sql);

if ($result->num_rows ==1) {
		while($row = $result->fetch_assoc()) {
			//echo "type: " . $row["type"]. " - Name: " . $row["firstname"]. " " . $row["lastname"]. "<br>";
			//echo " welcome :" . $row["type"];
				if($row["type"]=="agent" or $_POST['auth']=="yes" ){
					//agent sequance
					agent_sequence($conn,$number,$time);
					
				}else{
					echo" that's an article but you must identify your self first";
				}
	    }
} else {
    echo "unknown rfcode id ";
}

/*
$result = $conn->query($sql);

if ($result->num_rows ==1) {
    // output data of each row
    while($row = $result->fetch_assoc()) {
        //echo "type: " . $row["type"]. " - Name: " . $row["firstname"]. " " . $row["lastname"]. "<br>";
		echo " welcome :" . $row["type"];
    }
} else {
    echo "0 results";
}*/

$conn->close();

}


?>