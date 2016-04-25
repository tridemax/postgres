<?php

//-------------------------------------------------------------------------------------------------
$wowPluginDatabase = array(
  'sql_host' => 'localhost',
  'sql_database' => 'host1450090_wowplugin',
  'sql_user' => 'host1450090',
  'sql_pass' => '472b1e86',
  'sql_port' => 3306,
  'sql_socket' => '/var/lib/mysql/mysql.sock'
);

//-------------------------------------------------------------------------------------------------
function InstallWowPluginDatabase($dummy)
{
	$dbConnection = mysqli_connect($wowPluginDatabase['sql_host'], $wowPluginDatabase['sql_user'], $wowPluginDatabase['sql_pass'], $wowPluginDatabase['sql_database']);

	if (!$dbConnection)
	{
		return false;
	}

	header('Content-Type: text/plain');
	echo(mysqli_get_host_info($dbConnection));

	mysqli_close($dbConnection);

	return true;
}

?>
