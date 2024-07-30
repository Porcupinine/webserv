#!/bin/bash

# The URL of your server
URL="http://localhost:8080"

# Number of requests to send
REQUESTS=10000

for i in $(seq 1 $REQUESTS); do
  curl $URL &  # The '&' puts the task in the background
done

wait # Wait for all background tasks to complete
echo "All requests completed."
