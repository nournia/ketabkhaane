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

if (empty($_POST['id']) or empty($_POST['key']) or empty($_POST['actions']) or empty($_POST['time']))
{
	echo 'error - invalid arguments';
	exit;
}

$qry = "select * from libraries where id = {$_POST['id']} and sha1(concat(group_id, '-', license)) = '{$_POST['key']}'";
$library = mysql_fetch_array(mysql_query($qry));
if (empty($library))
{
	echo 'error - invalid access';
	exit;
}

$dependents = array(
	'users' => array('matches'=>'designer_id', 'answers'=> 'user_id', 'payments'=>'user_id', 'open_scores'=>'user_id', 'supports'=>'corrector_id', 'permissions'=>'user_id'),
	'matches' => array('questions'=>'match_id', 'answers'=>'match_id', 'supports'=>'match_id'),
	'authors' => array('resources'=>'author_id'),
	'publications' => array('resources'=>'publication_id'),
	'resources' => array('matches'=>'resource_id')
);

if (!empty($_POST['actions']))
{
	// get posted json data and decode them
	$actions = json_decode($_POST['actions']); // utf8_decode($_POST['json']);
	
	$defaultUpdatedAt = "concat('1971-01-01 ', sec_to_time({$library['id']}))";
	$transitions = array();
	foreach ($actions as $command => $data)
	foreach ($data as $table => $rows)
	{
		$insert = $command == 'insert';
	
		// get table columns
		$columns = ''; $libraryValues = '';
		$result = mysql_query("show columns from {$table}");
		while ($row = mysql_fetch_array($result))
		{
			$columns .= (! empty($columns) ? ',' : '') . $row[0];
			if ($row[0] == 'tournament_id')
				$libraryValues .= $library['tournament_id'] .',';
			else if ($row[0] == 'group_id')
				$libraryValues .= $library['group_id'] .',';
		}
	
		foreach($rows as $row)
		{
			$values = ($command == 'insert' ? 'null' : $row[0]) . ',';
			// for tournament tables like payments start with "1," and else with ""
			$values .= $libraryValues; // tournament_id must be the second field
			for($i = 1; $i < count($row); $i++)
				$values .= getQueryValue($row[$i]) .',';
			$values .= $insert ? $defaultUpdatedAt : 'now()'; // updated_at field
	
			mysql_query($command . " into {$table} ({$columns}) values ({$values})");
	
			if ($insert)
			{
				// push transition in array and at the end print it in direct or reverse order
				$lastId = $row[0]; $newId = mysql_insert_id();
				$transitions[$table][$lastId] = $newId;
	
				if (! empty($dependents[$table]))
				foreach ($dependents[$table] as $dependent => $key)
					mysql_query("update {$dependent} set {$key} = {$newId} where {$key} = {$lastId} and updated_at = {$defaultUpdatedAt}");
			}
		}
	
		// if insert part is not empty
		if ($insert)
		{
			// print new ids for record; client will update last id to this new one
			$output = '';
			if ($lastId < $newId) // reverse order
				foreach ($transitions[$table] as $lid => $nid)
					$output = $lid .'-'. $nid .',' . $output;
			else // direct order
				foreach ($transitions[$table] as $lid => $nid)
					$output .= $lid .'-'. $nid .',' ;
	
			if (! empty($output))
				echo $table .','. $output .'/'; 
		}
	}
	
	$fileDir = $_SERVER['DOCUMENT_ROOT'] . 'files/';
	foreach ($_FILES as $file)
	{
		$name = explode('.', $file['name']); 
		if (isset($transitions['files'][$name[0]]))
			move_uploaded_file($file['tmp_name'], $fileDir . $transitions['files'][$name[0]] .'.'. $name[1]);
	}
	
	if (isset($_POST['finished']))
	foreach (array('files', 'permissions', 'answers', 'questions', 'payments', 'supports', 'open_scores', 'matches', 'resources', 'authors', 'publications', 'users') as $table)
			mysql_query("update {$table} set updated_at = now() where updated_at = {$defaultUpdatedAt}");
	
	mysql_query("update libraries set synced_at = '{$_POST['time']}' where id = {$library['id']}");
	echo (isset($_POST['finished']) ? '*' : '+') . $_POST['time'];
}

disconnectDatabase();
?>