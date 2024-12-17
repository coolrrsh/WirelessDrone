#!/bin/sh

sleep 1

# Infinite loop to run ./a.out repeatedly
while true; do
    ./a.out
    sleep 5  # Optional: Add a small delay between runs if needed
done

