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

function getTableColumns($table)
{
	$columns = '';
	$result = mysql_query("show columns from {$table}");
	while ($row = mysql_fetch_array($result))
	if ($row[0] != 'id')
		$columns .= (! empty($columns) ? ',' : '') . $row[0];
	return $columns;
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


	if (! empty($_POST['create']))
	{
		// get posted json data and decode them
		$json = $_POST['create']; // utf8_decode($_POST['json']);
		$data = json_decode($json);

		//echo $json; 	print_r($data); die;
		
		// retreive trournament_id
		$tournament_id = 1;
				
		foreach ($data as $table => $rows)
		{
			$columns = getTableColumns($table);
			
			foreach($rows as $row)
			{
				$values = '';
				for($i = 1; $i < count($row); $i++)
					$values .= getQueryValue($row[$i]) .',';
				$values .= 'NOW()'; // updated_at field
				
				mysql_query("insert into {$table} ({$columns}) values ({$values})");
				
				// print new id for record -- client will update last id to this new one
				echo $row[0] .'\t'. mysql_insert_id() .'\n';
				//mysql_query("replace into {$table} values ({$values})");
			}
		}
		
		echo 'ok';
	} else
		echo 'invalid access';
		
	disconnectDatabase();
?>
</pre>
</body>
</html>