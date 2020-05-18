<?php

require __DIR__ . '/../vendor/autoload.php';
require __DIR__ . '/../source/API/Framework.php.inc';

use SolarEgdeAPI\API;
use Longman\TelegramBot;

/* Check if file key exists */
if (!file_exists(__DIR__ . '/../config/api_key.key')) {
    exit('API key not found' . PHP_EOL);
}

/* Try to get content */
$apiKey = file_get_contents(__DIR__ . '/../config/api_key.key');
if ($apiKey === FALSE) {
    exit('Failed to retrieve key' . PHP_EOL);
}

/* Check if file telegram exists */
if (!file_exists(__DIR__ . '/../config/telegram.json')) {
    exit('Telegram config not found' . PHP_EOL);
}

/* Try to get content */
$telegramConfig = file_get_contents(__DIR__ . '/../config/telegram.json');
if ($telegramConfig === FALSE) {
    exit('Failed to retrieve telegram config' . PHP_EOL);
}

/* Try to decode config */
$telegramConfig = json_decode($telegramConfig, TRUE);
if ($telegramConfig === FALSE || empty($telegramConfig['apiKey']) || empty($telegramConfig['botName'])) {
    exit('Failed to retrieve telegram config' . PHP_EOL);
}

/* Init framework */
$apiKey = trim(preg_replace('/\s+/', ' ', $apiKey));
$framework = new API\Framework($apiKey);

/* Get sites */
$sites = $framework->getSiteList();
if ($sites == NUll) {
    exit($framework->getLastError());
}

try {
    /* Create Telegram API object */
    $telegram = new TelegramBot\Telegram($telegramConfig['apiKey'], $telegramConfig['botName']);

    /* Loop each site */
    foreach ($sites as $site) {
        /* Get data period */
        $dataPeriodObject = $site->getDataPeriodObject();
        $startDate = $dataPeriodObject->startDate->format('Y-m-d');
        $endDate = $dataPeriodObject->endDate->format('Y-m-d');
        $filepath = __DIR__ . '/../exports/' . $endDate . '.png';

        /* Set default info */
        $text ='Site: ' . $site->getName() . PHP_EOL;
        $text .='Data period:' . PHP_EOL;
        $text .= $startDate . ' - ' . $endDate . PHP_EOL;

        /* Split data */
        $text .='--------------------------' . PHP_EOL;

        /* Create energy list */
        $text .='Energy year:' . PHP_EOL;
        $energyList = $site->getEnergy($startDate, $endDate);
        if ($energyList !== NULL) {
            foreach ($energyList as $energy) {
                $text .= $energy->getDateObject()->format('Y') . ': ' . $energy->getFormatValue(API\Energy::UNIT_MWH) . PHP_EOL;
            }
        }

        /* Split data */
        $text .='--------------------------' . PHP_EOL;

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
            $site->convertToBarDiagram($filepath, $energyList, '\W\k W');
        }

         /* Create energy list */
        $text .= 'Energy this month (' . API\Energy::ConvertFormatValue($monthTotal, API\Energy::UNIT_KWH) . '):' . PHP_EOL;
        $text .= $monthText;

        /* Split data */
        $text .='--------------------------' . PHP_EOL;

        /* Try to get information about yesterday */
        $yEndDateObject = clone $dataPeriodObject->endDate;
        $yEndDateObject->modify('-1 day');
        $yesterday = $yEndDateObject->format('Y-m-d');
        $energyList = $site->getEnergy($yesterday, $yesterday, API\Site::TIME_UNIT_DAY);
        if (!empty($energyList)) {
            $text .= 'Yesterday energy (' . $energyList[0]->getDateObject()->format('Y-m-d') . '): ' . $energyList[0]->getFormatValue(API\Energy::UNIT_KWH) . PHP_EOL;
        }

        /* Try to send messages */
        if (!empty($telegramConfig['chatIds']) && is_array($telegramConfig['chatIds'])) {
            $fileExists = file_exists($filepath);
            foreach ($telegramConfig['chatIds'] as $chatId) {
                /* Send meessage */
                $result = TelegramBot\Request::sendMessage([
                    'chat_id' => $chatId,
                    'text' => $text
                ]);

                /* Check if export file exists */
                $telegramPhoto = $fileExists ? TelegramBot\Request::encodeFile($filepath) : NULL;
                if ($telegramPhoto !== NULL) {
                    $result = TelegramBot\Request::sendPhoto([
                        'chat_id' => $chatId,
                        'photo' => $telegramPhoto
                    ]);
                }
            }
        }
    }
} catch (TelegramBot\Exception\TelegramException $e) {
    exit($e->getMessage());
}
