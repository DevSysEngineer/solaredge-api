<?php

require '../source/API/Framework.php.inc';

use SolarEgdeAPI\API;

/* Check if file key exists */
if (!file_exists('../api_key.key')) {
    exit('API key not found' . PHP_EOL);
}

/* Try to get content */
$apiKey = file_get_contents('../api_key.key');
if ($apiKey === FALSE ) {
    exit('Failed to retrieve key' . PHP_EOL);
}

/* Init framework */
$apiKey = trim(preg_replace('/\s+/', ' ', $apiKey));
$framework = new API\Framework($apiKey);

/* Get sites */
$sites = $framework->getSiteList();
if ($sites == NUll) {
    exit($framework->getLastError());
}

foreach ($sites as $site) {
    /* Get data period */
    $dataPeriod = $site->getDataPeriod();
    $endDateObject = new DateTime($dataPeriod->endDate);

    /* Set default info */
    echo 'Site: ' . $site->getName() . PHP_EOL;
    echo 'Data period from: ' . $dataPeriod->startDate . ' until ' . $dataPeriod->endDate . PHP_EOL;

    /* Split data */
    echo '----------------------------------------------' . PHP_EOL;

    /* Create energy list */
    echo 'Energy year list:' . PHP_EOL;
    $energyList = $site->getEnergy($dataPeriod->startDate, $dataPeriod->endDate);
    if ($energyList !== NULL) {
        foreach ($energyList as $energy) {
            echo $energy->getDate() . ': ' . $energy->getFormatValue(API\Energy::UNIT_MWH) . PHP_EOL;
        }
    }

    /* Split data */
    echo '----------------------------------------------' . PHP_EOL;

    /* Try to get information about yesterday */
    $endDateObject->modify('-1 day');
    $yesterday = $endDateObject->format('Y-m-d');
    $energyList = $site->getEnergy($yesterday, $yesterday, API\Site::TIME_UNIT_DAY);
    if (!empty($energyList)) {
        echo 'Yesterday energy: ' . $energyList[0]->getDate() . ': ' . $energyList[0]->getFormatValue(API\Energy::UNIT_KWH) . PHP_EOL;
    }
}

