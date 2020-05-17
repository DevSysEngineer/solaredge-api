<?php

require __DIR__ . '/../source/API/Framework.php.inc';

use SolarEgdeAPI\API;

/* Check if file key exists */
if (!file_exists(__DIR__ . '/../config/api_key.key')) {
    exit('API key not found' . PHP_EOL);
}

/* Try to get content */
$apiKey = file_get_contents(__DIR__ . '/../config/api_key.key');
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

/* Loop each site */
foreach ($sites as $site) {
    /* Get data period */
    $dataPeriodObject = $site->getDataPeriodObject();
    $startDate = $dataPeriodObject->startDate->format('Y-m-d');
    $endDate = $dataPeriodObject->endDate->format('Y-m-d');

    /* Set default info */
    echo 'Site: ' . $site->getName() . PHP_EOL;
    echo 'Data period from: ' . $startDate . ' until ' . $endDate . PHP_EOL;

    /* Split data */
    echo '----------------------------------------------' . PHP_EOL;

    /* Create energy list */
    echo 'Energy year:' . PHP_EOL;
    $energyList = $site->getEnergy($startDate, $endDate);
    if ($energyList !== NULL) {
        foreach ($energyList as $energy) {
            echo $energy->getDateObject()->format('Y') . ': ' . $energy->getFormatValue(API\Energy::UNIT_MWH) . PHP_EOL;
        }
    }

    /* Split data */
    echo '----------------------------------------------' . PHP_EOL;


    /* Try to get correct start en end date */
    $mEndDateObject = clone $dataPeriodObject->endDate;
    $mEndDateObject->modify('first day of this month');
    $firstDayOfMonth = $mEndDateObject->format('Y-m-d');
    $mEndDateObject->modify('last day of this month');
    $lastDayOfMonth = $mEndDateObject->format('Y-m-d');

    /* Create month text */
    $monthText = '';
    $monthTotal = 0.0;
    $energyList = $site->getEnergy($firstDayOfMonth, $lastDayOfMonth, API\Site::TIME_UNIT_WEEK);
    if ($energyList !== NULL) {
        /* Convvert to text */
        foreach ($energyList as $energy) {
            $monthTotal += $energy->getValue(API\Energy::UNIT_KWH);
            $monthText .= 'Week ' . $energy->getDateObject()->format('W') . ': ' . $energy->getFormatValue(API\Energy::UNIT_KWH) . PHP_EOL;
        }

        /* Export also to PNG */
        $site->convertToBarDiagram(__DIR__ . '/../exports/' . $endDate . '.png', $energyList, '\W\k W');
    }

     /* Create energy list */
    echo 'Energy this month (' . API\Energy::ConvertFormatValue($monthTotal, API\Energy::UNIT_KWH) . '):' . PHP_EOL;
    echo $monthText;

    /* Split data */
    echo '----------------------------------------------' . PHP_EOL;

    /* Try to get information about yesterday */
    $yEndDateObject = clone $dataPeriodObject->endDate;
    $yEndDateObject->modify('-1 day');
    $yesterday = $yEndDateObject->format('Y-m-d');
    $energyList = $site->getEnergy($yesterday, $yesterday, API\Site::TIME_UNIT_DAY);
    if (!empty($energyList)) {
        echo 'Yesterday energy (' . $energyList[0]->getDateObject()->format('Y-m-d') . '): ' . $energyList[0]->getFormatValue(API\Energy::UNIT_KWH) . PHP_EOL;
    }
}
