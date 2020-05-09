<?php

namespace SolarEgdeAPI\API;

class URI {

    protected $_type = NULL;
    protected $_path = NULL;

    public function __construct(string $type, string $path) {
        $this->_type = $type;
        $this->_path = $path;
    }

    public function doRequest(array $arguments = []) {
        /* Explode path */
        $expl = explode('/', $this->_path);
        if (empty($expl) || count($expl) === 1) {
            return NULL;
        }

        /* Get type */
        $type = $expl[0];
        unset($expl[0]);

        /* Get last index */
        $index = array_key_last($expl);
        if (empty($index)) {
            return NULL;
        }

        /* Get information */
        $information = $expl[$index];
        unset($expl[$index]);

        /* Do request */
        return Framework::DoRequest($type, implode('/', $expl), $information);
    }
}
