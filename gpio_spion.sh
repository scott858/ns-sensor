#!/bin/bash
#GPIO7 => GPIO02 (for real) https://docs.toradex.com/101770-apalis-imx6-datasheet.pdf, https://developer.toradex.com/knowledge-base/gpio-alphanumeric-to-gpio-numeric-assignment
echo 1 > /sys/class/gpio/gpio2/value

