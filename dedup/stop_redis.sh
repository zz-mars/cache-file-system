#!/bin/bash

kill $(ps aux | grep redis | awk 'NR==1{print $2}') > /dev/null

exit 0

