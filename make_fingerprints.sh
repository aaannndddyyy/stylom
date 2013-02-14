#!/bin/bash

cat texts/dickens/* | ./stylom -n "Charles Dickens" -f > fingerprints/dickens.style
cat texts/austen/* | ./stylom -n "Jane Austen" -f > fingerprints/austen.style
cat texts/carroll/* | ./stylom -n "Lewis Carroll" -f > fingerprints/carroll.style
cat texts/wilde/* | ./stylom -n "Oscar Wilde" -f > fingerprints/wilde.style
