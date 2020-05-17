<?php

namespace SolarEgdeAPI\API;

use SolarEgdeAPI\Diagram;

class Site {

    const TIME_UNIT_YEAR = 'YEAR';
    const TIME_UNIT_DAY = 'DAY';
    const TIME_UNIT_WEEK = 'WEEK';

    protected $_id = NULL;
    protected $_name = NULL;
    protected $_accountId = NULL;
    protected $_status = NULL;
    protected $_peakPower = NULL;
    protected $_lastUpdateTime = NULL;
    protected $_installationDate = NULL;
    protected $_ptoDate = NULL;
    protected $_notes = NULL;
    protected $_type = NULL;
    protected $_location = NULL;
    protected $_uris = [];

    public function __construct(\stdClass $site) {
        $this->_id = $site->id;
        $this->_name = $site->name;
        $this->_accountId = $site->accountId;
        $this->_status = $site->status;
        $this->_peakPower = $site->peakPower;
        $this->_lastUpdateTime = $site->lastUpdateTime;
        $this->_installationDate = $site->installationDate;
        $this->_ptoDate = $site->ptoDate;
        $this->_notes = $site->notes;
        $this->_type = $site->type;
        $this->_location = new Location($site->location);

        /* Get URLS */
        foreach ($site->uris as $key => $path) {
            $this->_uris[$key] = new URI($key, $path);
        }
    }

    public function getName() : ?string {
        return $this->_name;
    }

    public function getDataPeriod() : ?\stdClass {
        /* Check if uri exist */
        if (empty($this->_uris['DATA_PERIOD'])) {
            return NULL;
        }

        /* Do request */
        $output = $this->_uris['DATA_PERIOD']->doRequest();
        if ($output === NULL) {
            return NULL;
        }

        /* Return data */
        return $output->dataPeriod;
    }

    public function getDataPeriodObject() : ?\stdClass {
        /* Try to get object */
        $object = $this->getDataPeriod();
        if ($object === NULL) {
            return NULL;
        }

        /* Convert data */
        $timeZone = $this->_location->getTimeZone();
        $object->startDate = new \DateTime($object->startDate, $timeZone);
        $object->endDate = new \DateTime($object->endDate, $timeZone);

        /* Return date objecr */
        return $object;
    }

    public function getEnergy($startDate, $endDate, $timeUnit = self::TIME_UNIT_YEAR) : ?array {
        /* Do request */
        $output = Framework::DoRequest('site', $this->_id, 'energy', ['startDate' => $startDate, 'endDate' => $endDate, 'timeUnit' => $timeUnit]);
        if ($output === NULL) {
            return NULL;
        }

        /* Get energies */
        $energyList = [];
        foreach ($output->energy->values as $energy) {
            $energyList[] = new Energy($output->energy->unit, $output->energy->measuredBy, $energy, $this->_location->getTimeZone());
        }

        /* Return result */
        return $energyList;
    }

    public function convertToBarDiagram(string $path, array $energyList, string $dateFormat, int $width = 400, int $heigth = 300) : bool {
        /* Create object */
        $bar = new Diagram\Bar($width, $heigth);

        /* Add energy list to diagram */
        foreach ($energyList as $energy) {
            if (!$energy instanceof Energy) {
                continue;
            }

            /* Apply data */
            $value = $energy->getValue(Energy::UNIT_KWH);
            $friendlyValue = Energy::ConvertFormatValue($value, Energy::UNIT_KWH);
            $bar->addData($value, $friendlyValue, $energy->getDateObject()->format($dateFormat));
        }

        /* Build diagram */
        return $bar->build($path);
    }
}
