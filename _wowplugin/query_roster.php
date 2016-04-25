<?php

require_once('conf_global.php');
require_once('auxiliary.php');

define('ROSTER_CACHE_UNAVAILABLE', 0);
define('ROSTER_CACHE_EXPIRED', 1);
define('ROSTER_CACHE_OK', 2);

//http://render-api-eu.worldofwarcraft.com/static-render/eu/booty-bay/104/146705256-avatar.jpg
//"character": {"thumbnail": "booty-bay/206/14758094-avatar.jpg"}

//-------------------------------------------------------------------------------------------------
class WowCharacter
{
	public $name = '';
	public $realm = '';
	public $race = 0;
	public $gender = 0;
}

//-------------------------------------------------------------------------------------------------
class WowRoster
{
	public $paladins = array();
	public $warriors = array();
	public $shamans = array();
	public $hunters = array();
	public $druids = array();
	public $rogues = array();
	public $monks = array();
	public $mages = array();
	public $priests = array();
	public $warlocks = array();
	public $deathknights = array();
	public $demonhunters = array();
}

//-------------------------------------------------------------------------------------------------
function GetUrlContent($url, &$httpResponse)
{
	$curlResource = curl_init();
	curl_setopt($curlResource, CURLOPT_URL, $url);
	curl_setopt($curlResource, CURLOPT_USERAGENT, 'Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; .NET CLR 1.1.4322)');
	curl_setopt($curlResource, CURLOPT_RETURNTRANSFER, 1);
	curl_setopt($curlResource, CURLOPT_CONNECTTIMEOUT, 5);
	curl_setopt($curlResource, CURLOPT_TIMEOUT, 5);

//	curl_setopt($curlResource, CURLOPT_SSL_VERIFYPEER, 0);

	$httpResponse = curl_exec($curlResource);
	$httpError = curl_error($curlResource);
	$httpCode = curl_getinfo($curlResource, CURLINFO_HTTP_CODE);

	if (empty($httpResponse) && !empty($httpError))
	{
		$httpResponse = $httpError;
		curl_close($curlResource);
		return false;
	}

	if (!($httpCode >= 200 && $httpCode < 400))
	{
		$httpResponse = 'HTTP error: ' . strval($httpCode);
		curl_close($curlResource);
		return false;
	}

	curl_close($curlResource);

	return true;
}

//-------------------------------------------------------------------------------------------------
function LoadRosterFromArmory($requestedGuild, $requestedGuildRealm, &$armorySnapshot)
{
	$wowApiKey = 'j2j53pgx4vx8p2562hmzph6rnrwcdpna';

	$armoryUrl =  'https://eu.api.battle.net/wow/guild/' . $requestedGuildRealm . '/' . $requestedGuild . '?fields=members&locale=en_GB&apikey=' . $wowApiKey;

	return GetUrlContent($armoryUrl, $armorySnapshot);
}

//-------------------------------------------------------------------------------------------------
function BuildResponseFromArmorySnapshot($armorySnapshot, $requestedGuildRanks, &$response)
{
	$decodedSnapshot = json_decode($armorySnapshot);

	// Check the snapshot
	if (!$decodedSnapshot)
	{
		$response = 'Unable to parse an armory snapshot.';
		return false;
	}

	if (!isset($decodedSnapshot->members))
	{
		$response = 'Server encountered odd armory snapshot.';
		return false;
	}

	// Scan the snapshot and build a response
	$outputRoster = new WowRoster();

	foreach ($decodedSnapshot->members as $member)
	{
		if (count($requestedGuildRanks) != 0 && !in_array($member->rank, $requestedGuildRanks))
		{
			continue;
		}

		$newWowCharacter = new WowCharacter();
		$newWowCharacter->name = $member->character->name;
		$newWowCharacter->realm = $member->character->realm;
		$newWowCharacter->race = $member->character->race;
		$newWowCharacter->gender = $member->character->gender;

		switch ($member->character->class)
		{
			case 2:
				$outputRoster->paladins[] = $newWowCharacter;
				break;
			case 1:
				$outputRoster->warriors[] = $newWowCharacter;
				break;
			case 7:
				$outputRoster->shamans[] = $newWowCharacter;
				break;
			case 3:
				$outputRoster->hunters[] = $newWowCharacter;
				break;
			case 11:
				$outputRoster->druids[] = $newWowCharacter;
				break;
			case 4:
				$outputRoster->rogues[] = $newWowCharacter;
				break;
			case 10:
				$outputRoster->monks[] = $newWowCharacter;
				break;
			case 8:
				$outputRoster->mages[] = $newWowCharacter;
				break;
			case 5:
				$outputRoster->priests[] = $newWowCharacter;
				break;
			case 9:
				$outputRoster->warlocks[] = $newWowCharacter;
				break;
			case 6:
				$outputRoster->deathknights[] = $newWowCharacter;
				break;
		}
	}

	$response = json_encode($outputRoster);

	return true;
}

//-------------------------------------------------------------------------------------------------
function LoadRosterFromDatabase($requestedGuild, $requestedGuildRealm, &$roster)
{
}

//-------------------------------------------------------------------------------------------------
function QueryRosterCacheState($requestedGuild, $requestedGuildRealm)
{
	return ROSTER_CACHE_UNAVAILABLE;
}

//-------------------------------------------------------------------------------------------------
// Entry point

$requestBody = file_get_contents('php://input');

if (empty($requestBody))
{
	CustomError('Request body is empty.');
	exit(0);
}

$requestBodyDecoded = json_decode($requestBody);

if (empty($requestBodyDecoded))
{
	CustomError('Unable to decode the request body.');
	exit(0);
}

if (empty($requestBodyDecoded->guild) || empty($requestBodyDecoded->realm))
{
	CustomError('Not specified guild or guild realm.');
	exit(0);
}

$requestedGuild = $requestBodyDecoded->guild;
$requestedGuildRealm = $requestBodyDecoded->realm;
$requestedGuildRanks = isset($requestBodyDecoded->ranks) ? $requestBodyDecoded->ranks : array();

$encodedResponse = '';

switch (QueryRosterCacheState($requestedGuild, $requestedGuildRealm))
{
	case ROSTER_CACHE_UNAVAILABLE:
		$armorySnapshot = '';

		if (!LoadRosterFromArmory($requestedGuild, $requestedGuildRealm, $armorySnapshot))
		{
			CustomError($armorySnapshot);
			exit(0);
		}

		if (!BuildResponseFromArmorySnapshot($armorySnapshot, $requestedGuildRanks, $encodedResponse))
		{
			CustomError($encodedResponse);
			exit(0);
		}
		break;

	case ROSTER_CACHE_EXPIRED:
		break;

	case ROSTER_CACHE_OK:
		break;
}

header('Content-Type: application/json; charset=UTF-8');
echo($encodedResponse);

?>
