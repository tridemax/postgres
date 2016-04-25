<?php

require_once('conf_global.php');
require_once('auxiliary.php');

define('NEWSLINE_ENTRY_UNKNOWN', 0);
define('NEWSLINE_ENTRY_TEXT', 1);
define('NEWSLINE_ENTRY_IMAGE', 2);
define('NEWSLINE_ENTRY_YOUTUBE', 3);

//-------------------------------------------------------------------------------------------------
class WowNewslineEntry
{
	public $type = NEWSLINE_ENTRY_UNKNOWN;
	public $date = null;
	public $title = '';
	public $contentList = array();
}

//-------------------------------------------------------------------------------------------------
class WowNewsline
{
	public $entries = array();
	public $totalEntriesQuantity = 0;
}

//-------------------------------------------------------------------------------------------------
function TempGetWowNewslineEntries($skip, $limit, &$outputNewsline, &$totalEntriesQuantity)
{
	// Source newsline
	$sourceNewsline = array();

	$entry10 = new WowNewslineEntry();
	$entry10->type = NEWSLINE_ENTRY_YOUTUBE;
	$entry10->date = new DateTime();
	$entry10->title = 'Mythic Archimonde';
	$entry10->contentList[] = '72ZdfvaL-jw';
	$sourceNewsline[] = $entry10;

	$entry0 = new WowNewslineEntry();
	$entry0->type = NEWSLINE_ENTRY_YOUTUBE;
	$entry0->date = new DateTime();
	$entry0->title = 'Mythic Mannoroth';
	$entry0->contentList[] = 'Ccjhiw75IkE';
	$sourceNewsline[] = $entry0;

	$entry1 = new WowNewslineEntry();
	$entry1->type = NEWSLINE_ENTRY_YOUTUBE;
	$entry1->date = new DateTime();
	$entry1->title = 'Mythic Xhul\'horac';
	$entry1->contentList[] = 'ObX0AYb7Fqw';
	$sourceNewsline[] = $entry1;

	$entry2 = new WowNewslineEntry();
	$entry2->type = NEWSLINE_ENTRY_YOUTUBE;
	$entry2->date = new DateTime();
	$entry2->title = 'Mythic Tyrant Velhari';
	$entry2->contentList[] = 'BVPY2-VfL8s';
	$sourceNewsline[] = $entry2;

	$entry3 = new WowNewslineEntry();
	$entry3->type = NEWSLINE_ENTRY_YOUTUBE;
	$entry3->date = new DateTime();
	$entry3->title = 'Mythic Fel Lord Zakuun';
	$entry3->contentList[] = '7P7hRDY4OyQ';
	$entry3->contentList[] = '6CAbb2RMwCg';
	$sourceNewsline[] = $entry3;

	$entry4 = new WowNewslineEntry();
	$entry4->type = NEWSLINE_ENTRY_YOUTUBE;
	$entry4->date = new DateTime();
	$entry4->title = 'Mythic Socrethar the Eternal';
	$entry4->contentList[] = 'V8sxN5f3gFU';
	$sourceNewsline[] = $entry4;

	$entry5 = new WowNewslineEntry();
	$entry5->type = NEWSLINE_ENTRY_YOUTUBE;
	$entry5->date = new DateTime();
	$entry5->title = 'Mythic Gorefiend';
	$entry5->contentList[] = 'pOaliHtr1cY';
	$sourceNewsline[] = $entry5;

	// Output newsline
	$startIndex = min($skip, count($sourceNewsline) - 1);
	$entriesToCopy = min($limit, count($sourceNewsline) - $startIndex);

	$outputNewsline = array();

	for ($entryIndex = 0; $entryIndex != $entriesToCopy; ++$entryIndex)
	{
		$outputNewsline[] = $sourceNewsline[$startIndex + $entryIndex];
	}

	// Setup statistics
	$totalEntriesQuantity = count($sourceNewsline);

	return true;
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

if (!isset($requestBodyDecoded->skip) || !isset($requestBodyDecoded->limit))
{
	CustomError('Not specified skip or limit for newsline entries.');
	exit(0);
}

$outputNewsline = new WowNewsline();

if (!TempGetWowNewslineEntries($requestBodyDecoded->skip, $requestBodyDecoded->limit, $outputNewsline->entries, $outputNewsline->totalEntriesQuantity))
{
	CustomError($outputNewsline);
	exit(0);
}

$encodedResponse = json_encode($outputNewsline);

header('Content-Type: application/json; charset=UTF-8');
echo($encodedResponse);

?>
