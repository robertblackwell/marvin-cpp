#!/bin/bash
# curl -H "Connection: close" -x localhost:9992 http://whiteacorn/utests/echo/index.php
curl --insecure --raw https://localhost:9992/
