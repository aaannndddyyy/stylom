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

/* sort the vocabulary into alphabetical order */
static void sort_vocabulary(char * vocab, float * frequency)
{
    int i, j, k;
    float freq;
    char temp[MAX_WORD_LENGTH];

    for (i = 0; i < 26; i++) {
        for (j = 0; j < MAX_VOCAB_SIZE; j++) {
            if (vocab[VINDEX(i,j)]==0) break;
            for (k = j+1; k < MAX_VOCAB_SIZE; k++) {
                if (vocab[VINDEX(i,k)]==0) break;
                if (strcmp(&vocab[VINDEX(i,j)],
                           &vocab[VINDEX(i,k)]) > 0) {
                    /* swap word */
                    memcpy((void*)temp,(void*)&vocab[VINDEX(i,j)],
                           MAX_WORD_LENGTH*sizeof(char));

                    memcpy((void*)&vocab[VINDEX(i,j)],
                           (void*)&vocab[VINDEX(i,k)],
                           MAX_WORD_LENGTH*sizeof(char));

                    memcpy((void*)&vocab[VINDEX(i,k)], (void*)temp,
                           MAX_WORD_LENGTH*sizeof(char));

                    /* swap frequency */
                    freq = frequency[FINDEX(i,j)];
                    frequency[FINDEX(i,j)] = frequency[FINDEX(i,k)];
                    frequency[FINDEX(i,k)] = freq;
                }
            }
        }
    }
}

/* initialise the vocabulary */
static void init_vocabulary(char * vocab)
{
    int i,j;

    for (i = 0; i < 26; i++) {
        for (j = 0; j < MAX_VOCAB_SIZE; j++) {
            vocab[VINDEX(i,j)] = 0;
        }
    }
}

/* initialise the vocabulary word frequencies */
static void init_vocabulary_frequency(float * frequency)
{
    memset((void*)frequency,'\0',26*MAX_VOCAB_SIZE*sizeof(float));
}

/* add a word to the vocabulary */
static int add_word_to_vocabulary(char * text,
                                  int start_pos, int end_pos,
                                  char * vocab,
                                  float * frequency)
{
    int index,i,c;
    int length = end_pos-start_pos;

    if (length >= MAX_WORD_LENGTH-1) return 0;

    if ((text[start_pos]>='a') && (text[start_pos]<='z')) {
        index = (int)(text[start_pos]-'a');
    }
    else {
        index = (int)(text[start_pos]-'A');
    }
    if (index >= 26) return 0;

    i=0;
    while (i < MAX_VOCAB_SIZE) {
        if (vocab[VINDEX(index,i)]==0) break;
        i++;
    }
    if (i < MAX_VOCAB_SIZE) {
        for (c = 0; c < length; c++) {
            vocab[VINDEX(index,i)+c] = tolower(text[start_pos+c]);
        }
        vocab[VINDEX(index,i)+c] = 0;
        frequency[FINDEX(index,i)] = 1;
        return 1;
    }
    return 0;
}

/* returns 1 if the given word is in the vocabulary */
static int word_in_vocabulary(char * text,
                              int start_pos, int end_pos,
                              char * vocab, float * frequency,
                              int update_frequency,
                              float * returned_frequency)
{
    int length = end_pos - start_pos;
    int index,i=0,c;

    if ((length==0) || (length>=MAX_WORD_LENGTH-1)) return 0;

    if ((text[start_pos]>='a') && (text[start_pos]<='z')) {
        index = (int)(text[start_pos]-'a');
    }
    else {
        index = (int)(text[start_pos]-'A');
    }
    if (index >= 26) return 0;

    while (i < MAX_VOCAB_SIZE) {
        if (vocab[VINDEX(index,i)]==0) break;
        if (strlen(&vocab[VINDEX(index,i)]) == length) {
            for (c = start_pos; c < end_pos; c++) {
                if (tolower(text[c]) !=
                        vocab[VINDEX(index,i) + c - start_pos]) break;
            }
            if (c == end_pos) {
                if (update_frequency > 0) {
                    frequency[FINDEX(index,i)]++;
                }
                else {
                    *returned_frequency =
                        frequency[FINDEX(index,i)];
                }
                return 1;
            }
        }
        i++;
    }
    return 0;
}

/* compares one vocabulary against another and returns
   lists of missing words, ones which are more and less frequent */
int compare_vocabulary(char * vocab1, char * vocab2,
                       float * frequency1, float * frequency2,
                       char * missing,
                       char * more, char * less,
                       float threshold,
                       int line_length)
{
    int i,j,c,index,ctr;
    char * wordstr;
    float freq1,freq2,min;
    int missing_ctr=0;
    int more_words_ctr=0;
    int less_words_ctr=0;
    int word_length;
    char more_words[MAX_WORD_LENGTH*MAX_COMPARE_RESULTS];
    float more_words_freq[MAX_COMPARE_RESULTS];
    char less_words[MAX_WORD_LENGTH*MAX_COMPARE_RESULTS];
    float less_words_freq[MAX_COMPARE_RESULTS];
    int total_difference=0;

    /* clear the more and less words lists */
    for (i = 0; i < MAX_COMPARE_RESULTS; i++) {
        more_words[i*MAX_WORD_LENGTH] = 0;
        less_words[i*MAX_WORD_LENGTH] = 0;
    }

    /* for each word in the first vocabulary */
    ctr=0;
    for (i = 0; i < 26; i++) {
        for (j = 0; j < MAX_VOCAB_SIZE; j++) {
            if (vocab1[VINDEX(i,j)]==0) break;
            /* get the word */
            wordstr = &vocab1[VINDEX(i,j)];
            word_length = strlen(wordstr);
            /* get the frequency of the word */
            freq1 = frequency1[FINDEX(i,j)];

            /* does this word exist in the second vocabulary? */
            if (word_in_vocabulary(wordstr, 0, strlen(wordstr),
                                   vocab2, frequency2, 0,&freq2)==0) {
                total_difference++;
                if (missing_ctr < 512) {
                    /* add word to the missing list */
                    for (c = 0; c < word_length; c++) {
                        missing[missing_ctr++] = wordstr[c];
                        ctr++;
                    }
                    missing[missing_ctr++] = ' ';
                    ctr++;
                    if (ctr>line_length) {
                        missing[missing_ctr++] = '\n';
                        ctr=0;
                    }
                }
            }
            else {
                if (freq2 - freq1 > threshold) {
                    /* more */
                    if (more_words_ctr < MAX_COMPARE_RESULTS) {
                        /* pick the next index in the sequence */
                        index = more_words_ctr;
                        more_words_ctr++;
                    }
                    else {
                        /* pick the word with the biggest
                           frequency difference */
                        min = 9999;
                        index = -1;
                        for (c = 0; c < MAX_COMPARE_RESULTS; c++) {
                            if (more_words_freq[c]>freq2-freq1) {
                                if (more_words_freq[c]<min) {
                                    min = more_words_freq[c];
                                    index = c;
                                }
                            }
                        }
                    }
                    if (index>-1) {
                        /* update to the more words */
                        for (c = 0; c < word_length; c++) {
                            more_words[index*MAX_WORD_LENGTH+c]=
                                wordstr[c];
                        }
                        more_words[index*MAX_WORD_LENGTH+c]=0;
                        more_words_freq[index] = freq2-freq1;
                        total_difference++;
                    }
                }
                if (freq1 - freq2 > threshold) {
                    /* less */
                    if (less_words_ctr < MAX_COMPARE_RESULTS) {
                        /* pick the next index in the sequence */
                        index = less_words_ctr;
                        less_words_ctr++;
                    }
                    else {
                        /* pick the word with the biggest
                           frequency difference */
                        min = 9999;
                        index = -1;
                        for (c = 0; c < MAX_COMPARE_RESULTS; c++) {
                            if (less_words_freq[c]>freq1-freq2) {
                                if (less_words_freq[c]<min) {
                                    min = less_words_freq[c];
                                    index = c;
                                }
                            }
                        }
                    }
                    if (index>-1) {
                        /* update to the less words */
                        for (c = 0; c < word_length; c++) {
                            less_words[index*MAX_WORD_LENGTH+c]=
                                wordstr[c];
                        }
                        less_words[index*MAX_WORD_LENGTH+c]=0;
                        less_words_freq[index] = freq1-freq2;
                        total_difference++;
                    }
                }
            }
        }
    }

    /* terminate the missing words list */
    missing[missing_ctr] = 0;

    /* update the more words */
    ctr = 0;
    index = 0;
    for (i = 0; i < more_words_ctr; i++) {
        wordstr = &more_words[i*MAX_WORD_LENGTH];
        for (c = 0; c < strlen(wordstr); c++) {
            more[index++] = wordstr[c];
            ctr++;
        }
        more[index++] = ' ';
        ctr++;
        if (ctr > line_length) {
            ctr = 0;
            more[index++] = '\n';
        }
    }
    more[index] = 0;

    /* update the less words */
    ctr = 0;
    index = 0;
    for (i = 0; i < less_words_ctr; i++) {
        wordstr = &less_words[i*MAX_WORD_LENGTH];
        for (c = 0; c < strlen(wordstr); c++) {
            less[index++] = wordstr[c];
            ctr++;
        }
        less[index++] = ' ';
        ctr++;
        if (ctr > line_length) {
            ctr = 0;
            less[index++] = '\n';
        }
    }
    less[index] = 0;

    return total_difference;
}

/* creates a vocabulary for the given text */
int create_vocabulary(char * text, char * vocab, float * frequency)
{
    int i, state=1, prev_pos=-1, max=1;
    int length = strlen(text)-1;
    int total_words = 0;

    init_vocabulary(vocab);
    init_vocabulary_frequency(frequency);

    for (i = 0; i < length; i++) {
        if (state == 0) {
            /* look for the start of a word */
            if (((text[i]>='a') && (text[i]<='z')) ||
                    ((text[i]>='A') && (text[i]<='Z'))) {
                /* change state to look for the end of a word */
                state = 1;
                prev_pos = i;
            }
        }
        if (state == 1) {
            /* look for the end of a word */
            if (word_terminator(text[i],text[i+1]) == 1) {
                /* change state to look for the start of a word */
                state = 0;
                if (prev_pos > -1) {
                    if (word_in_vocabulary(text, prev_pos, i+1,
                                           vocab, frequency,1,0)==0) {
                        if (add_word_to_vocabulary(text,
                                                   prev_pos, i+1,
                                                   vocab,
                                                   frequency)==1) {
                            total_words++;
                        }
                    }
                }
            }
        }
    }

    sort_vocabulary(vocab, frequency);

    /* normalise word frequencies */
    for (i = 0; i < 26*MAX_VOCAB_SIZE; i++) {
        if (frequency[i]>max) max = frequency[i];
    }
    for (i = 0; i < 26*MAX_VOCAB_SIZE; i++) {
        frequency[i] /= max;
    }

    return total_words;
}

/* parse the given text string from an XML file and update the
   vocabulary */
void parse_vocabulary_string(char * text, char* vocab)
{
    int i,c,ctr=0,index,entry;
    int length = strlen(text);
    char value_str[256];

    init_vocabulary(vocab);

    for (i = 0; i < length; i++) {
        if ((text[i] >= 'a') && (text[i] <= 'z') &&
                (i < length-1)) {
            value_str[ctr++] = text[i];
        }
        else {
            if (ctr > 0) {
                if ((i == length-1) &&
                        (text[i] >= 'a') && (text[i] <= 'z')) {
                    value_str[ctr++] = text[i];
                }

                value_str[ctr] = 0;
                index = (int)(tolower(value_str[0])-'a');
                entry = 0;
                while (vocab[VINDEX(index,entry)]!=0) {
                    entry++;
                    if (entry==MAX_VOCAB_SIZE) break;
                }
                if (entry < MAX_VOCAB_SIZE) {
                    for (c = 0; c <= ctr; c++) {
                        vocab[VINDEX(index,entry) + c] =
                            value_str[c];
                    }
                }
            }
            ctr = 0;
        }
    }
}

/* parse the given text string from an XML file and update the
   vocabulary */
void parse_vocabulary_frequency_string(char * text,
                                       char * vocab,
                                       float * frequency)
{
    int i,ctr=0,index=0,entry=0,found;
    int length = strlen(text);
    char value_str[256];

    init_vocabulary_frequency(frequency);

    for (i = 0; i < length; i++) {
        if ((((text[i] >= '0') && (text[i] <= '9')) ||
                (text[i] == '.') || (text[i] == ',')) &&
                (i < length-1)) {
            value_str[ctr++] = text[i];
        }
        else {
            if (ctr > 0) {
                if ((i == index-1) &&
                        (((text[i] >= '0') && (text[i] <= '9')) ||
                         (text[i] == '.') || (text[i] == ','))) {
                    value_str[ctr++] = text[i];
                }
                value_str[ctr] = 0;

                found = 0;
                while ((found == 0) && (index<26)) {
                    if (vocab[VINDEX(index,entry)]!=0) {
                        found = 1;
                        frequency[FINDEX(index,entry)] =
                            atof(value_str);
                    }
                    entry++;
                    if (entry==MAX_VOCAB_SIZE) {
                        index++;
                        entry = 0;
                    }
                }
            }
            ctr = 0;
        }
    }
}

/* saves the vocabulary in XML format */
void write_vocabulary_xml(char* vocab, float * frequency, FILE * fp)
{
    int i,j,length=0;
    char * str;

    fprintf(fp,"%s","  <vocabulary>\n  ");

    for (i = 0; i < 26; i++) {
        for (j = 0; j < MAX_VOCAB_SIZE; j++) {
            if (vocab[VINDEX(i,j)]==0) break;
            str = &vocab[VINDEX(i,j)];
            length += strlen(str);
            fprintf(fp,"%s ",str);
            if (length > CONSOLE_WIDTH) {
                fprintf(fp,"%s","\n  ");
                length = 0;
            }
        }
    }

    fprintf(fp,"%s","\n  </vocabulary>\n");

    fprintf(fp,"%s","  <vocabularyfreq>\n  ");

    length = 0;
    for (i = 0; i < 26; i++) {
        for (j = 0; j < MAX_VOCAB_SIZE; j++) {
            if (vocab[VINDEX(i,j)]==0) break;
            length += 11;
            fprintf(fp,"%.8f ",frequency[FINDEX(i,j)]);
            if (length > CONSOLE_WIDTH) {
                fprintf(fp,"%s","\n  ");
                length = 0;
            }
        }
    }

    fprintf(fp,"%s","\n  </vocabularyfreq>\n");

}

int create_dictionary(char * text, float dictionary[])
{
    int i, state=1, prev_pos=-1, index, total_words=0;
    int length = strlen(text)-1;
    float max=1, mult=DICTIONARY_SIZE/(26.0f*1.02f);

    /* clear the histogram */
    memset((void*)dictionary, '\0', DICTIONARY_SIZE*sizeof(float));

    for (i = 0; i < length; i++) {
        if (state == 0) {
            /* look for the start of a word */
            if (((text[i]>='a') && (text[i]<='z')) ||
                    ((text[i]>='A') && (text[i]<='Z'))) {
                /* change state to look for the end of a word */
                state = 1;
                prev_pos = i;
            }
        }
        if (state == 1) {
            /* look for the end of a word */
            if (word_terminator(text[i],text[i+1])==1) {
                /* change state to look for the start of a word */
                state = 0;
                if (prev_pos>-1) {
                    index = (int)word_to_float(text,
                                               prev_pos, i+1,
                                               mult);
                    if (index >= DICTIONARY_SIZE) {
                        printf("Dictionary size exceeded %d/%d\n",
                               index,DICTIONARY_SIZE);
                        return -1;
                    }
                    else {
                        dictionary[index]++;
                        total_words++;
                    }
                }
            }
        }
    }

    /* find the maximum value */
    for (i = 0; i < DICTIONARY_SIZE; i++) {
        if (dictionary[i]>max) max = dictionary[i];
    }

    /* normalise */
    for (i = 0; i < DICTIONARY_SIZE; i++) {
        dictionary[i] /= max;
    }
    return total_words;
}
