/*
    Stylom: A command line utility for stylometric text analysis
    Copyright (C) 2013 Bob Mottram
    fuzzgun@gmail.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "stylom.h"

/* detect whether this is the end of a word */
int word_terminator(char ch1, char ch2)
{
    if (((ch1>='a') && (ch1<='z')) ||
            ((ch1>='A') && (ch1<='Z'))) {
        if (!(((ch2>='a') && (ch2<='z')) ||
                ((ch2>='A') && (ch2<='Z')))) {
            /* probably the end */
            return 1;
        }
    }
    /* not the end */
    return 0;
}

/* returns a floating point value corresponding to a word */
float word_to_float(char * text,
                    int start_pos, int end_pos,
                    float mult)
{
    float value = 0;
    int i, v;

    for (i = start_pos; i < end_pos; i++) {
        if (((text[i]>='a') && (text[i]<='z')) ||
                ((text[i]>='A') && (text[i]<='Z'))) {
            if ((text[i]>='A') && (text[i]<='Z')) {
                v = (int)(text[i]-'A');
            }
            else {
                v = (int)(text[i]-'a');
            }
            value += mult * v;
            mult /= 26;
        }
    }
    return value;
}
