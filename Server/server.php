<!DOCTYPE html>
<html lang="fa">
<head>
<meta charset="utf-8" />
</head>
<body>
<pre>
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
function getQueryValue($value)
{
	if (isset($value))
		return "'". mysql_real_escape_string($value) . "'";
	else return 'null';
}
?>
<?php
	connectDatabase();
	
	/*
	$result = mysql_query('select * from users');
	while ($row = mysql_fetch_row($result))
		print_r($row);
	die;
	 */

	// get posted json data and decode them
	$json = $_POST['json']; // utf8_decode($_POST['json']);
	$data = json_decode($json);
	
	foreach ($data as $table => $rows)
	foreach($rows as $row)
	{
		$values = '';
		foreach($row as $column)
			$values .= getQueryValue($column) .',';
		$values .= 'NOW()'; // updated_at field
			
		//$qry = "insert into {$table} values ({$values})";
		//echo $qry;
		
		mysql_query("insert into {$table} values ({$values})");
	}
	
	disconnectDatabase();
?>
</pre>
</body>
</html>