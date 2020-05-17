<?php

namespace SolarEgdeAPI\Diagram;

class Bar {

    protected $_asLineColor = [92, 184, 92];
    protected $_outlineBarColor = [66, 139, 202];
    protected $_insideBarColor = [91, 192, 222];
    protected $_textColor = [217, 83, 79];

    protected $_data = [];
    protected $_friendlyData = [];
    protected $_shortLabel = [];

    public function __construct(int $widht = 500, int $height = 200) {
        $this->_width = $widht;
        $this->_height = $height;
    }

    protected function _getTextHeight(int $dataWidth, int $fontSize, string $font, string $text, bool $force) : array {
        /* Get text width and compare */
        $result = $force ? [0, 0, 0, 0, 0, 0, 0, 0] : imagettfbbox($fontSize, 0, $font, $text);
        if ($force || $result[2] > $dataWidth) { // lower right corner (X)
            /* Get result */
            $result = imagettfbbox($fontSize, 270, $font, $text);
            $textWidth = abs($result[4]); // upper right corner (X)
            $textHeight = abs($result[5]); // upper right corner (Y)

            /* Check if height is divide by two */
            if ($textWidth % 2 > 0) {
                $textWidth += 1;
            }

            /* Check if height is divide by two */
            if ($textHeight % 2 > 0) {
                $textHeight += 1;
            }

            /* Return result */
            return ['height', $textHeight, $textWidth];
        } else {
            /* Get width */
            $textWidth = abs($result[2]); // lower right corner (X)
            if ($textWidth % 2 > 0) {
                $textWidth += 1;
            }

            /* Get height */
            $textHeight =  abs($result[7]); // upper left corner (Y)
            if ($textHeight % 2 > 0) {
                $textHeight += 1;
            }

            /* Return result */
            return ['width', $textHeight, $textWidth];
        }
    }

    public function addData(int $value, string $friendlyValue = NULL, $shortLabel = NULL) {
        $this->_data[] = $value;
        $this->_friendlyData[] = $friendlyValue;
        $this->_shortLabel[] = $shortLabel;
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
        $margeTop = 0;
        $margeBottom = 0;
        $margeText = 2;
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

        /* Set some value */
        $newArray = [];
        $textHorizontal = TRUE;
        $shortLabelHorizontal = TRUE;
        $textHeightCorrection = 0;
        $shortLabelHeightCorrection = 0;

        /* Loop data */
        for ($key = 0; $key < count($this->_data); $key++) {
            /* Chekc if value is positive */
            $value = $this->_data[$key];
            if ($value <= 0) {
                continue;
            }

            /* Check text width and height */
            $text = empty($this->_friendlyData[$key]) ? strval($value) : $this->_friendlyData[$key];
            $textResult = $this->_getTextHeight($dataWidth, $fontSize, $font, $text, !$textHorizontal);
            list($type, $height, $width) = $textResult;
            switch ($type) {
                case 'width':
                    if ($height > $textHeightCorrection) {
                        $textHeightCorrection = $height;
                    }
                    break;
                case 'height':
                    if ($height > $textHeightCorrection) {
                        /* Set height */
                        $textHeightCorrection = $height;

                        /* Check if we need to restart building the array */
                        if ($textHorizontal) {
                            $textHorizontal = FALSE;
                            if ($key !== 0) {
                                $newArray = [];
                                $key--;
                                continue 2;
                            }
                        }
                    }
                    break;
            }

            /* Set value */
            $object = new \stdClass;
            $object->type = 'main';
            $object->index = $key;
            $object->text = $text;
            $object->value = $value;
            $object->textWidth = $width;
            $object->textHeight = $height;

             /* Check if sort label is given */
            if (!empty($this->_shortLabel[$key])) {
                 /* Set text */
                $shortLabel = new \stdClass;
                $shortLabel->text = $this->_shortLabel[$key];

                /* Get Hiehgt */
                $textResult = $this->_getTextHeight($dataWidth, $fontSize, $font, $this->_shortLabel[$key], !$shortLabelHorizontal);
                list($type, $height, $width) = $textResult;
                switch ($type) {
                    case 'width':
                        if ($height > $shortLabelHeightCorrection) {
                            $shortLabelHeightCorrection = $height;
                        }
                        break;
                    case 'height':
                        if ($height > $shortLabelHeightCorrection) {
                            /* Set height */
                            $shortLabelHeightCorrection = $height;

                            /* Check if we need to restart building the array */
                            if ($shortLabelHorizontal) {
                                $shortLabelHorizontal = FALSE;
                                if ($key !== 0) {
                                    $newArray = [];
                                    $key--;
                                    continue 2;
                                }
                            }
                        }
                        break;
                }

                /* Set with and height */
                $shortLabel->textWidth = $width;
                $shortLabel->textHeight = $height;

                /* Set object */
                $object->shortLabel = $shortLabel;
            } else {
                $object->shortLabel = NULL;
            }

            /* Set object */
            $newArray[] = $object;
        }

        /* Change if text height correction is applied */
        if ($textHeightCorrection > 0) {
            $textHeightCorrection += (2 * $margeText);
            $actualHeight -= $textHeightCorrection;
        }

        /* Change if label short height correction is applied */
        if ($shortLabelHeightCorrection > 0) {
            $shortLabelHeightCorrection += (2 * $margeText);
            $actualHeight -= $shortLabelHeightCorrection;
        }

        /* Create y-ax line */
        $yAs = $margeTop + $shortLabelHeightCorrection + $actualHeight;
        imageline($image, $margeLeft, 0, $margeLeft, $yAs, $asLineColorAllocate);

        /* Create x-ax line */
        $xAs = $margeTop + $shortLabelHeightCorrection + $actualHeight;
        imageline($image, $margeLeft, $xAs, $this->_width, $xAs, $asLineColorAllocate);

        /* First x and y coordinate */
        $xAs = $margeLeft + $margeBetween;

        /* Get highest number */
        $highestNumber = 0.00;
        foreach ($newArray as $object) {
            if ($object->value > $highestNumber) {
                $highestNumber = $object->value;
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
        foreach ($newArray as $object) {
            /* Set some value */
            $correctXAs = $xAs + $dataWidth;
            $correctYAs = $margeTop + $shortLabelHeightCorrection + $actualHeight;
            $yAs = $correctYAs - floor($object->value * $formFactor);

            /* Check if short label is given */
            if ($object->shortLabel !== NULL) {
                /* Set some value */
                $shortLabelAngle = $shortLabelHorizontal ? 0 : 270;
                $shortLabelXAs = $xAs + floor(($dataWidth - $object->shortLabel->textWidth) / 2);
                $shortLabelYAs = $yAs - $margeText;
                if (!$shortLabelHorizontal) {
                    $shortLabelYAs -= $object->shortLabel->textHeight;
                }

                /* Draw texr */
                imagettftext($image, 8, $shortLabelAngle, $shortLabelXAs, $shortLabelYAs, $textColorAllocate, $font, $object->shortLabel->text);
            }

            /* Draw horizon top line */
            imageline($image, $xAs, $yAs, $correctXAs, $yAs, $outlineBarColorAllocate);

            /* Draw left side */
            imageline($image, $xAs, $yAs, $xAs, $correctYAs, $outlineBarColorAllocate);

            /* Draw right side */
            imageline($image, $correctXAs, $yAs, $correctXAs, $correctYAs, $outlineBarColorAllocate);

            /* Draw a rectangle */
            imagefilledrectangle($image, $xAs + 1, $yAs + 1, $correctXAs - 1, $correctYAs - 1, $insideBarColorAllocate);

            /* Set text */
            $fontSize = 8;

            /* Draw text */
            $angle = $textHorizontal ? 0 : 270;
            $textXAs = $xAs + floor(($dataWidth - $object->textWidth) / 2);
            $textYAs = $correctYAs + floor(($this->_height - $correctYAs + $object->textHeight) / 2);
            imagettftext($image, $fontSize, $angle, $textXAs, $textYAs, $textColorAllocate, $font, $object->text);

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
