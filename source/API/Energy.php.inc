<?php

namespace SolarEgdeAPI\API;

class Energy {

    const UNIT_UNKNOWN= 0;
    const UNIT_WH = 1;
    const UNIT_KWH = 1000;
    const UNIT_MWH = 1000000;

    protected $_unit = NULL;
    protected $_measuredBy = NULL;
    protected $_date = NULL;
    protected $_value = NULL;
    protected $_timeZone = NULL;

    public function __construct(string $unit, string $measuredBy, \stdClass $energy, \DateTimeZone $timeZone) {
        $this->_unit = $this->_translateUnitToInt($unit);
        $this->_measuredBy = $measuredBy;
        $this->_date = $energy->date;
        $this->_value = intval($energy->value);
        $this->_timeZone = $timeZone;
    }

    protected static function _TranslateIntToUnit(int $unit) : string {
        switch ($unit) {
            case self::UNIT_WH:
                return 'Wh';
            case self::UNIT_KWH:
                return 'KWh';
            case self::UNIT_MWH:
                return 'MWh';
            default:
                return 'unknown';
        }
    }

    public static function ConvertFormatValue(float $value, int $unit, int $round = 2) {
        return number_format(round($value, $round), 2, '.', '') . self::_TranslateIntToUnit($unit);
    }

    protected function _translateUnitToInt(string $unit) : int {
        switch ($unit) {
            case 'Wh':
                return self::UNIT_WH;
            default:
                return self::UNIT_UNKNOWN;
        }
    }

    public function getDate() : ?string {
        return $this->_date;
    }

    public function getDateObject() : \DateTime {
        return new \DateTime($this->_date, $this->_timeZone);
    }

    public function getValue(int $unit = self::UNIT_WH) : ?float {
        /* Check if value or unit is NULL */
        if ($this->_value === NULL || $this->_unit === NULL) {
            return NULL;
        }

        /* Check if unit is the same */
        if ($unit === $this->_unit) {
            return $this->_value;
        }

        /* Convert value */
        switch ($this->_unit) {
            case self::UNIT_WH:
                if ($unit > $this->_unit) {
                    $value = $this->_value / $unit;
                } else {
                    $value = $this->_value;
                }
                break;
            default:
                $value = $this->_value;
        }

        /* Return correct value */
        return floatval($value);
    }

    public function getFormatValue(int $unit = self::UNIT_WH, int $round = 2) : ?string {
        /* Check if value is correcrt */
        $value = $this->getValue($unit);
        if ($value === NULL) {
            return NULL;
        }

        /* Convert value */
        return self::ConvertFormatValue($value, $unit, $round);
    }
}
