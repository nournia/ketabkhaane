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

function getTableColumns($table, & $tournamentRow)
{
	$columns = ''; $tournamentRow = false;
	$result = mysql_query("show columns from {$table}");
	while ($row = mysql_fetch_array($result))
	{
		if ($row[0] != 'id')
			$columns .= (! empty($columns) ? ',' : '') . $row[0];
		if ($row[0] == 'tournament_id')
			$tournamentRow = true;
	}
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

		// echo $json; 	print_r($data); die;
		
		// retreive trournament_id
		$tournament_id = 1;
				
		foreach ($data as $table => $rows)
		{
			$columns = getTableColumns($table, $tournamentRow);
			$tournamentValue = $tournamentRow ? $tournament_id .',' : '';

			echo $table .',';			
			foreach($rows as $row)
			{
				// for tournament tables like payments start with "1," and else with ""
				$values = $tournamentValue;
				
				for($i = 1; $i < count($row); $i++)
					$values .= getQueryValue($row[$i]) .',';
				$values .= 'NOW()'; // updated_at field
				
				mysql_query("insert into {$table} ({$columns}) values ({$values})");
				
				// print new id for record -- client will update last id to this new one
				echo $row[0] .'-'. mysql_insert_id() .',';
				//mysql_query("replace into {$table} values ({$values})");
			}
			echo '/'; // end of table
		}
	} else
		echo 'invalid access';
		
	disconnectDatabase();
?>