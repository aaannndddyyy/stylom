#!/bin/bash

# Generates packaging for stylom

packagemonkey -n "stylom" --cmd --dir "." -l "gpl3" -e "Bob Mottram (4096 bits) <bob@robotics.uk.to>" --brief "Command line utility for stylometric text analysis" --desc "Tool to facilitate the stylometric analysis of texts. It could be used for academic disambiguation of disputed authorship, and to help identify plagiarists, astroturfers, sockpuppets and guerilla marketers. Another possible use case is as an assistance to the anonymisation of writing style.  Every author has their own unique writing style, and if enough writing examples are available then it's possible to construct a quantitative model of their style which can be compared against others." --homepage "https://github.com/fuzzgun/stylom" --repository "https://github.com/fuzzgun/stylom.git" --section "utils" --version "1.00" --categories "Utility/ConsoleOnly" --dependsdeb "gnuplot" --dependsrpm "gnuplot" --dependsarch "gnuplot" --dependspuppy "gnuplot_4.4.3" --compile "-lm"
