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
	
	// debug print_r($_POST); die;	
	
	$dependents = array(
		'users' => array('matches'=>'designer_id', 'answers'=> 'user_id', 'payments'=>'user_id', 'open_scores'=>'user_id', 'supports'=>'corrector_id'),
		'matches' => array('questions'=>'match_id', 'answers'=>'match_id', 'supports'=>'match_id'),
		'authors' => array('resources'=>'author_id'),
		'publications' => array('resources'=>'publication_id'),
		'resources' => array('matches'=>'resource_id')
	);

	if (empty($_POST['id']) or empty($_POST['key']) or empty($_POST['time']))
	{
		echo 'error - invalid arguments';
		exit;
	}

	$qry = "select tournament_id from libraries where id = {$_POST['id']} and concat(group_id, '-', license) = '{$_POST['key']}'";
	$library = mysql_fetch_array(mysql_query($qry));
	if (empty($library))
	{
		echo 'error - invalid access';
		exit;
	}
	
	// retreive trournament_id
	$tournament_id = $library['tournament_id'];
	
	if (! empty($_POST['create']))
	{
		// get posted json data and decode them
		$json = $_POST['create']; // utf8_decode($_POST['json']);
		$data = json_decode($json);

		foreach ($data as $table => $rows)
		{
			$columns = getTableColumns($table, $tournamentRow);
			$tournamentValue = $tournamentRow ? $tournament_id .',' : '';

			$transitions = array();
			echo $table .',';
			foreach($rows as $row)
			{
				// for tournament tables like payments start with "1," and else with ""
				$values = $tournamentValue;
				
				for($i = 1; $i < count($row); $i++)
					$values .= getQueryValue($row[$i]) .',';
				$values .= 'null'; //'NOW()'; // updated_at field
				
				mysql_query("insert into {$table} ({$columns}) values ({$values})");
				
				// push transition in array and at the end print it in direct or reverse order
				$lastId = $row[0]; $newId = mysql_insert_id();
				array_push($transitions, $lastId .'-'. $newId .',');
				
				if (! empty($dependents[$table]))
				foreach ($dependents[$table] as $dependent => $key)
					mysql_query("update {$dependent} set {$key} = {$newId} where {$key} = {$lastId}");
			}
			
			// print new ids for record; client will update last id to this new one
			$count = count($transitions);
			if ($lastId < $newId) // reverse order
				for ($i = $count-1; $i >= 0; $i--)
					echo $transitions[$i];
			else // direct order
				for ($i = 0; $i < $count; $i++)
					echo $transitions[$i];
			
			echo '/'; // end of table
		}
	}
	
	echo $_POST['time'];
	
	disconnectDatabase();
	//mysql_query("replace into {$table} values ({$values})");
?>