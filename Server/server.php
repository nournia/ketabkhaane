<?php 
function connectDatabase()
{
	$connection = mysql_connect("localhost", "root");
	if (!$connection) die("Connection faild: " . mysql_error());
	$db_select = mysql_select_db("reghaabat_development", $connection);
	if (!$db_select) die("Selection faild: " . mysql_error());
}
function disconnectDatabase()
{
	global $connection;
	if (isset($connection))
	mysql_close($connection);
}
?>
<?php
	connectDatabase();
	
	// get posted json data and decode them
	$data = json_decode(utf8_decode($_POST['json']));
	
	foreach ($data as $table => $rows)
	{
		$result = mysql_query("show columns from {$table}");
		while ($row = mysql_fetch_array($result))
			echo $row[0] .',';
	}
	
	disconnectDatabase();
?>
