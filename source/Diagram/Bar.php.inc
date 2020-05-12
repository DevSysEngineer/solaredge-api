<?php

namespace SolarEgdeAPI\Diagram;

class Bar {

    protected $_asLineColor = [92, 184, 92];
    protected $_outlineBarColor = [66, 139, 202];
    protected $_insideBarColor = [91, 192, 222];
    protected $_textColor = [217, 83, 79];

    protected $_data = [];
    protected $_friendlyData = [];

    public function __construct(int $widht = 500, int $height = 200) {
        $this->_width = $widht;
        $this->_height = $height;
    }

    public function addData(int $value, string $friendlyValue = NULL) {
        $this->_data[] = $value;
        $this->_friendlyData[] = $friendlyValue;
    }

    public function build(string $path) : bool {
        $image = @imagecreatetruecolor($this->_width, $this->_height);
        if ($image === FALSE) {
            return NULL;
        }

        /* important part one */
        imagesavealpha($image, true);
        imagealphablending($image, false);

        /* Important part two */
        $white = imagecolorallocatealpha($image, 255, 255, 255, 127);
        imagefill($image, 0, 0, $white);

        /* Create color for the lines */
        list($r, $g, $b) = $this->_asLineColor;
        $asLineColorAllocate = imagecolorallocate($image, $r, $g, $b);

        /* Create color for the lines */
        list($r, $g, $b) = $this->_outlineBarColor;
        $outlineBarColorAllocate = imagecolorallocate($image, $r, $g, $b);

        /* Create color for the lines */
        list($r, $g, $b) = $this->_insideBarColor;
        $insideBarColorAllocate = imagecolorallocate($image, $r, $g, $b);

        /* Create color for the lines */
        list($r, $g, $b) = $this->_textColor;
        $textColorAllocate = imagecolorallocate($image, $r, $g, $b);

        /* Settings */
        $margeLeft = 20;
        $margeBetween = 6;
        $margeTop = 10;
        $margeBottom = 20;
        $font = __DIR__ . '/Fonts/FreeSans.ttf';
        $fontSize = 8;

        /* Create y-ax line */
        $actualHeight = ($this->_height - $margeTop - $margeBottom);
        if ($actualHeight <= 0) {
            return FALSE;
        }

        /* Width of each data */
        $dataWidth = (($this->_width - $margeLeft) / count($this->_data)) - (2 * ($margeBetween / 2));
        $halfDataWith = $dataWidth / 2;

        /* Place text horizontal */
        $newArray = [];
        $textHorizontal = TRUE;
        $textCorrection = 0;
        foreach ($this->_data as $key => $value) {
            /* Chekc if value is positive */
            if ($value <= 0) {
                continue;
            }

            /* Check text width and height */
            $text = empty($this->_friendlyData[$key]) ? strval($value) : $this->_friendlyData[$key];
            $result = imagettfbbox($fontSize, 0, $font, $text);
            $textWidth = $result[2]; // lower right corner (X)
            if ($textWidth > $dataWidth) {
                $result = imagettfbbox($fontSize, 270, $font, $text);
                $textHeight = $result[5]; // upper right corner (Y)
                if ($textHeight > $textCorrection) {
                    $textCorrection = $textHeight;
                }
            }

            /* Set value */
            $newArray[$key] = $value;
        }

        /* Change if text correction is applied */
        if ($textCorrection > 0) {
            $textHorizontal = FALSE;
            $actualHeight -= $textCorrection;
        }

        /* Create y-ax line */
        imageline($image, $margeLeft, 0, $margeLeft, $margeTop + $actualHeight, $asLineColorAllocate);

        /* Create x-ax line */
        $xAs = $margeTop + $actualHeight;
        imageline($image, $margeLeft, $xAs, $this->_width, $xAs, $asLineColorAllocate);

        /* First x and y coordinate */
        $xAs = $margeLeft + $margeBetween;

        /* Get highest number */
        $highestNumber = 0.00;
        foreach ($this->_data as $value) {
            if ($value > $highestNumber) {
                $highestNumber = $value;
            }
        }

        /* Get form factor */
        $rawFactor = (float)($actualHeight / $highestNumber);
        if ($rawFactor >= 1.00 && $rawFactor < 2.00) {
            $formFactor = 1;
        } elseif ($rawFactor > 1) {
            $formFactor = floor($rawFactor);
        } else {
            $formFactor = $rawFactor;
        }

        /* Loop each data */
        foreach ($newArray as $key => $value) {
            /* Set some value */
            $correctXAs = $xAs + $dataWidth;
            $correctYAs = $margeTop + $actualHeight;

            /* Draw horizon top line */
            $yAs = $correctYAs - floor($value * $formFactor);
            imageline($image, $xAs, $yAs, $correctXAs, $yAs, $outlineBarColorAllocate);

            /* Draw left side */
            imageline($image, $xAs, $yAs, $xAs, $correctYAs, $outlineBarColorAllocate);

            /* Draw right side */
            imageline($image, $correctXAs, $yAs, $correctXAs, $correctYAs, $outlineBarColorAllocate);

            /* Draw a rectangle */
            imagefilledrectangle($image, $xAs + 1, $yAs + 1, $correctXAs - 1, $correctYAs - 1, $insideBarColorAllocate);

            /* Set text */
            $text = empty($this->_friendlyData[$key]) ? strval($value) : $this->_friendlyData[$key];
            $fontSize = 8;

            /* Get text information */
            $textXAs = $xAs;
            $textYAs = $correctYAs;
            $angle = $textHorizontal ? 0 : 270;
            $result = imagettfbbox($fontSize, $angle, $font, $text);
            if ($textHorizontal) {
                $textWidth = $result[2]; // lower right corner (X)
                $textHeight = $result[7]; // upper left corner (Y)
            } else {
                $textWidth = $result[4]; // upper right corner (X)
                $textHeight = $result[5]; // upper right corner (Y)
            }

            /* Draw text */
            $textXAs += floor(($dataWidth - $textWidth) / 2);
            $textYAs += floor(($this->_height - $correctYAs - $textHeight) / 2);
            imagettftext($image, $fontSize, $angle, $textXAs, $textYAs, $textColorAllocate, $font, $text);

            /* Update x-as */
            $xAs += $dataWidth + $margeBetween;
        }

        /* Try to write PNG to file */
        imagepng($image, $path);
        imagedestroy($image);

        /* Succes */
        return TRUE;
    }
}
