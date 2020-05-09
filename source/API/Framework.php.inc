<?php

namespace SolarEgdeAPI\API;

require 'URI.php.inc';
require 'Energy.php.inc';
require 'Site.php.inc';

class Framework {

    const URL = 'https://monitoringapi.solaredge.com/';

    protected static $_ApiKey = NULL;
    protected static $_LastError = NULL;

    public function __construct(string $apiKey) {
        self::$_ApiKey = $apiKey;
    }

    public static function DoRequest(string $type, string $id = NULL, $information = NULL, $arguments = []) : ?\stdClass {
        /* Create based url */
        $url = self::URL . $type;

        /* If ID is given, set IP */
        if ($id != NULL) {
            $url .= '/' . $id;
        }

        /* If information is given, set information */
        if ($information != NULL) {
            $url .= '/' . $information;
        }

        /* Set API key */
        $url .= '?api_key=' . urlencode(self::$_ApiKey);

        /* Check if arguments is not empty */
        if (!empty($arguments)) {
            $url .= '&' . http_build_query($arguments);
        }

        /* Create curl */
        $ch = curl_init();
        curl_setopt($ch, CURLOPT_URL, $url);
        curl_setopt($ch, CURLOPT_FAILONERROR, TRUE);
        curl_setopt($ch, CURLOPT_FOLLOWLOCATION, TRUE);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);

        /* Run request */
        $result = curl_exec($ch);
        if ($result === FALSE && curl_errno($ch)) {
            /* Set last error */
            self::$_LastError = 'CURL error (' . curl_errno($ch) . '): ' . curl_error($ch) . PHP_EOL;

            /* Failed */
            curl_close($ch);
            return NULL;
        }

        /* Close connection */
        curl_close($ch);

        /* Return output */
        return json_decode($result);
    }

    public function getLastError() {
        return self::$_LastError;
    }

    public function getSiteList() : ?array {
        /* Do request */
        $output = self::DoRequest('sites', NULL, 'list');
        if ($output === NULL) {
            return NULL;
        }

        /* Create sites */
        $sites = [];
        foreach ($output->sites->site as $site) {
            $sites[] = new Site($site);
        }

        /* Return sites */
        return $sites;
    }

}
