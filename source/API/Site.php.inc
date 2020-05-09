<?php

namespace SolarEgdeAPI\API;

class Site {

    const TIME_UNIT_YEAR = 'YEAR';
    const TIME_UNIT_DAY = 'DAY';

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

        /* Get URLS */
        foreach ($site->uris as $key => $path) {
            $this->_uris[$key] = new URI($key, $path);
        }
    }

    public function getName() {
        return $this->_name;
    }

    public function getDataPeriod() {
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

    public function getEnergy($startDate, $endDate, $timeUnit = self::TIME_UNIT_YEAR) {
        /* Do request */
        $output = Framework::DoRequest('site', $this->_id, 'energy', ['startDate' => $startDate, 'endDate' => $endDate, 'timeUnit' => $timeUnit]);
        if ($output === NULL) {
            return NULL;
        }

        /* Get energies */
        $energyList = [];
        foreach ($output->energy->values as $energy) {
            $energyList[] = new Energy($output->energy->unit, $output->energy->measuredBy, $energy);
        }

        /* Return result */
        return $energyList;
    }
}
