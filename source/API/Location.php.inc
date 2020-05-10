<?php

namespace SolarEgdeAPI\API;

class Location {

    protected $_country = NULL;
    protected $_city = NULL;
    protected $_address = NULL;
    protected $_address2 = NULL;
    protected $_zip = NULL;
    protected $_timeZone = NULL;
    protected $_countryCode = NULL;

    public function __construct(\stdClass $location) {
        $this->_country = $location->country;
        $this->_city = $location->city;
        $this->_address = $location->address;
        $this->_address2 = $location->address2;
        $this->_zip = $location->zip;
        $this->_timeZone = new \DateTimeZone($location->timeZone);
        $this->_countryCode = $location->countryCode;
    }

    public function getTimeZone() {
        return $this->_timeZone;
    }
}
