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

int read_text_from_file(char ** text, char * filename)
{
    FILE * fp;
    char linestr[4096];
    char * retval = NULL;
    int string_length = 0, i, ctr=0;

    fp = fopen(filename,"r");
    if (fp != NULL) {
        /* determine the length of the text needed */
        while (!feof(fp)) {
            retval = fgets(linestr,4095,fp);
            if (retval != NULL) {
                string_length += strlen(linestr);
            }
        }
        fclose(fp);
        /* if nothing was read */
        if (string_length == 0) {
            printf("Warning: nothing loaded\n");
            return 0;
        }
        /* allocate memory for the text */
        (*text) = (char*)malloc((string_length+1)*sizeof(char));
        if (!(*text)) {
            printf("read_text_from_file: Unable to allocate %d bytes\n",
                   string_length+1);
            return 0;
        }
        /* read the text into the array */
        fp = fopen(filename,"r");
        if (!fp) {
            printf("read_text_from_file: Unable to load %s\n",filename);
            free(*text);
            return 0;
        }
        while (!feof(fp)) {
            retval = fgets(linestr,4095,fp);
            if (retval != NULL) {
                for (i = 0; i < strlen(linestr); i++) {
                    (*text)[ctr++] = linestr[i];
                }
            }
        }
        fclose(fp);
        /* append string terminator */
        (*text)[ctr] = 0;
        return 1;
    }
    return 0;
}

/* frequencies of pairs of letters */
static void letter_pair_frequency(char * text, float histogram[])
{
    int i, index, x, y;
    float max = 1;

    /* clear the histogram */
    memset((void*)histogram, '\0',
		   MAX_LETTERS*MAX_LETTERS*sizeof(float));

    /* count letter pairs */
    for (i = strlen(text)-4; i >= 0; i--) {
        x = (int)text[i];
        if ((x >= 0) && (x < MAX_LETTERS)) {
			if ((x!=10) && (x!=13)) {

				/* first frequency */
				y = (int)text[i+1];
				if ((y >= 0) && (y < MAX_LETTERS)) {
					if ((y!=10) && (y!=13)) {
						index = (y*MAX_LETTERS) + x;
						histogram[index] += 1.0f;
						if (histogram[index] > max) {
							max = histogram[index];
						}
					}
				}
				/* second frequency */
				y = (int)text[i+2];
				if ((y >= 0) && (y < MAX_LETTERS)) {
					if ((y!=10) && (y!=13)) {
						index = (y*MAX_LETTERS) + x;
						histogram[index] += 0.5f;
						if (histogram[index] > max) {
							max = histogram[index];
						}
					}
				}
				/* third frequency */
				y = (int)text[i+3];
				if ((y >= 0) && (y < MAX_LETTERS)) {
					if ((y!=10) && (y!=13)) {
						index = (y*MAX_LETTERS) + x;
						histogram[index] += 0.2f;
						if (histogram[index] > max) {
							max = histogram[index];
						}
					}
				}
			}
        }
    }

    /* normalise */
    for (i = 0; i < MAX_LETTERS*MAX_LETTERS; i++) {
        if (histogram[i] > 0) {
            histogram[i] /= max;
        }
    }
}

/* creates a histogram based upon the first letters
   of successive words */
static void word_pair_frequency(char * text, float histogram[])
{
    int i, index, x, y, state=1;
    float max = 1;
    int length = strlen(text)-1;
    char prev_ch = ' ';
    char prev_ch2 = ' ';
    char prev_ch3 = ' ';

    /* clear the histogram */
    memset((void*)histogram, '\0', MAX_LETTERS*MAX_LETTERS*sizeof(float));

    for (i = 0; i < length; i++) {
        if (state == 0) {
            /* look for the start of a word */
            if (((text[i]>='a') && (text[i]<='z')) ||
                    ((text[i]>='A') && (text[i]<='Z'))) {
                /* change state to look for the end of a word */
                state = 1;
                if ((prev_ch != ' ') &&
                        (prev_ch2 != ' ') &&
                        (prev_ch3 != ' ')) {
                    x = (int)text[i];
                    if ((x >= 0) && (x < MAX_LETTERS)) {
                        /* first frequency */
                        y = (int)prev_ch;
                        if ((y >= 0) && (y < MAX_LETTERS)) {
                            index = (y*MAX_LETTERS) + x;
                            histogram[index] += 1.0f;
                            if (histogram[index] > max) {
                                max = histogram[index];
                            }
                        }
                        /* second frequency */
                        y = (int)prev_ch2;
                        if ((y >= 0) && (y < MAX_LETTERS)) {
                            index = (y*MAX_LETTERS) + x;
                            histogram[index] += 0.5f;
                            if (histogram[index] > max) {
                                max = histogram[index];
                            }
                        }
                        /* third frequency */
                        y = (int)prev_ch3;
                        if ((y >= 0) && (y < MAX_LETTERS)) {
                            index = (y*MAX_LETTERS) + x;
                            histogram[index] += 0.2f;
                            if (histogram[index] > max) {
                                max = histogram[index];
                            }
                        }
                    }
                }
                prev_ch3 = prev_ch2;
                prev_ch2 = prev_ch;
                prev_ch = text[i];
            }
        }
        if (state == 1) {
            /* look for the end of a word */
            if (word_terminator(text[i],text[i+1])==1) {
                /* change state to look for the start of a word */
                state = 0;
            }
        }
    }

    /* normalise */
    for (i = 0; i < MAX_LETTERS*MAX_LETTERS; i++) {
        if (histogram[i] > 0) {
            histogram[i] /= max;
        }
    }
}

/* creates a letter fequency histogram */
static void letter_frequency(char * text, float histogram[])
{
    int i, index;
    float max = 1;

    /* clear the histogram */
    memset((void*)histogram, '\0', MAX_LETTERS*sizeof(float));

    /* count letters */
    for (i = strlen(text)-1; i >= 0; i--) {
        index = (int)text[i];
        if ((index >= 0) && (index < MAX_LETTERS)) {
			if ((index!=10) && (index!=13)) {
				histogram[index]++;
			}
        }
    }

    /* find the maximum frequency */
    for (i = 0; i < MAX_LETTERS; i++) {
        if (histogram[i] > max) max = histogram[i];
    }

    /* normalise */
    for (i = 0; i < MAX_LETTERS; i++) {
        histogram[i] /= max;
    }
}

/* returns an estimate of the number of words within a given region */
static int words_count_region(char * text, int start_pos, int end_pos)
{
    int i,words=0;

    if (end_pos-start_pos < 2) return 0;

    for (i = start_pos; i < end_pos-1; i++) {
        if (word_terminator(text[i],text[i+1])!=0) {
            words++;
        }
    }
    return words;
}

/* create a histogram of word lengths */
static void word_length_histogram(char * text, float * histogram)
{
    int i, prev_pos=0,word_length,max=1;
    int length = strlen(text)-1;

    if (strlen(text) < 2) return;

    /* clear the histogram */
    memset((void*)histogram, '\0', MAX_WORD_LENGTH*sizeof(float));

    for (i = 0; i < length; i++) {
        if (word_terminator(text[i], text[i+1])!=0) {
            word_length = (i+1) - prev_pos;
            if ((word_length > 0) && (word_length < MAX_WORD_LENGTH)) {
                histogram[word_length]++;
                if (histogram[word_length] > max) {
                    max = histogram[word_length];
                }
            }
            prev_pos = i+1;
        }
    }

    /* normalise */
    for (i = 0; i < MAX_WORD_LENGTH; i++) {
        histogram[i] /= max;
    }
}

/* returns a histogram of sentence lengths */
static void sentence_length_histogram(char * text, float histogram[])
{
    int i, start_pos=0, end_pos, words, max=1;
    int length = strlen(text);

    /* clear the histogram */
    memset((void*)histogram, '\0',
		   MAX_WORDS_PER_SENTENCE*sizeof(float));

    for (i = 0; i < length; i++) {
        if ((text[i]=='.') || (i==length-1)) {
            end_pos = i;
            words = words_count_region(text, start_pos, end_pos);
            if ((words > 0) && (words < MAX_WORDS_PER_SENTENCE)) {
                histogram[words] = histogram[words] + 1;
                if (histogram[words] > max) {
                    max = histogram[words];
                }
            }
            start_pos = end_pos+1;
        }
    }

    /* normalise */
    for (i = 0; i < MAX_WORDS_PER_SENTENCE; i++) {
        histogram[i] /= max;
    }
}

/* returns a histogram of paragraph lengths */
static void paragraph_length_histogram(char * text, float histogram[])
{
    int i, start_pos=0, end_pos, words, max=1;
	int sentences=0, eol_ctr=0;
    int length = strlen(text);

    /* clear the histogram */
    memset((void*)histogram, '\0',
		   MAX_WORDS_PER_SENTENCE*sizeof(float));

    for (i = 0; i < length; i++) {
		if (text[i]==13) {
			eol_ctr++;
			if (eol_ctr > 1) {
				if ((sentences > 0) &&
					(sentences < MAX_SENTENCES_PER_PARAGRAPH)) {
					histogram[sentences] = histogram[sentences] + 1;
					if (histogram[sentences] > max) {
						max = histogram[sentences];
					}
				}
				eol_ctr = 0;
				sentences = 0;
			}
		}
        if ((text[i]=='.') || (i==length-1)) {
            end_pos = i;
            words = words_count_region(text, start_pos, end_pos);
            if ((words > 0) && (words < MAX_WORDS_PER_SENTENCE)) {
				sentences++;
				eol_ctr = 0;
            }
            start_pos = end_pos+1;
        }
    }

    /* normalise */
    for (i = 0; i < MAX_WORDS_PER_SENTENCE; i++) {
        histogram[i] /= max;
    }
}

/* finds a sentence of the given length and returns the line number.
 Returns -1 if no sentence of the given length is found */
static int find_sentence_of_length(char * text,
								   int sentence_length,
								   int * found_start,
								   int * found_end)
{
    int i, start_pos=0, end_pos, line_number = 0;
    int length = strlen(text);

    for (i = 0; i < length; i++) {
        if ((text[i]=='.') || (i==length-1)) {
            end_pos = i;
            if (words_count_region(text, start_pos, end_pos) ==
				sentence_length) {
				*found_start = start_pos;
				*found_end = end_pos;
				return line_number;
            }
            start_pos = end_pos+1;
        }
		if (text[i]==13) line_number++;
    }

	return -1;
}

/* compares sentence histograms and suggests changes to make f1
   more similar to f2 */
static int recommend_sentence_change(char * f1_text,
									 stylom_fingerprint * f1,
									 stylom_fingerprint * f2,
									 int * sentence_start,
									 int * sentence_end,
									 int * add_words)
{
	int i,j,length=0,length2=0,length3=0,line_number=-1;
	const int range = 3;
	float diff[2],max_diff=0,max;

	/* for every number of words */
	for (i = 7; i < MAX_WORDS_PER_SENTENCE; i++) {
		/* how many more instances are there in f2 than f1 ? */
		diff[0] = f2->sentence_length_histogram[i] -
			f1->sentence_length_histogram[i];
		/* if this the maximum difference ? */
		if (diff[0]*diff[0] > max_diff) {
			max = 0;
			length2 = -1;
			/* search within a range of a few words */
			for (j = -range+1; j < range; j++) {				
				if (range == 0) continue;
				/* range check */
				if ((i+j < 1) ||
					(i+j >= MAX_WORDS_PER_SENTENCE)) continue;
				/* how many more instances are there in f2 than f1 ? */
				diff[1] = f2->sentence_length_histogram[i+j] -
					f1->sentence_length_histogram[i+j];
				/* if this is the biggest difference */
				if (diff[1]*diff[1] > max) {
					/* ensure that the differences are
					   of opposite sign */
					if ((diff[0] < 0) && (diff[1] > 0)) {
						/* fewer instances in f1 than f2 */
						max = diff[1]*diff[1];
						length2 = i+j;
					}
					if ((diff[0] > 0) && (diff[1] < 0)) {
						/* more instances in f1 than f2 */
						max = diff[1]*diff[1];
						length2 = i+j;
					}
				}
			}
			if (length2 > -1) {
				max_diff = diff[0]*diff[0];
				length = i;
				length3 = length2;
			}
		}
	}

	if (length == 0) return -1;

	line_number = find_sentence_of_length(f1_text, length,
										  sentence_start,
										  sentence_end);
	if (line_number > -1) {
		*add_words = length3-length;
	}

	return line_number;
}
									   

/* returns a fingerprint for the given text */
void get_fingerprint(char * name, char * text,
					 stylom_fingerprint * fingerprint)
{
    /* set the author name */
    sprintf(fingerprint->name,"%s",name);

    /* calculate letter frequency histogram */
    letter_frequency(text, fingerprint->letter_histogram);

    /* calculate letter pair histogram */
    letter_pair_frequency(text, fingerprint->letter_pair_histogram);

    /* calculate word pair histogram */
    word_pair_frequency(text, fingerprint->word_pair_histogram);

    /* calculate sentence length histogram */
    sentence_length_histogram(text,
                              fingerprint->sentence_length_histogram);

    /* calculate paragraph length histogram */
    paragraph_length_histogram(text,
							   fingerprint->paragraph_length_histogram);

    /* calculate word length histogram */
    word_length_histogram(text, fingerprint->word_length_histogram);

    /* create a dictionary */
    create_dictionary(text, fingerprint->dictionary);

    /* create vocabulary */
    create_vocabulary(text, fingerprint->vocabulary,
                      fingerprint->vocabulary_frequency);
}

/* convert text extracted from xml into an array of floats */
static void string_to_float_array(char * text, float * floatarray)
{
    int i, index=0, ctr = 0, length = strlen(text);
    char value_str[256];

    for (i = 0; i < length; i++) {
        if (((text[i] == ' ') ||
                (text[i] == 13) ||
                (i == length-1)) && (ctr > 0)) {
            value_str[ctr] = 0;
            floatarray[index++] = atof(value_str);
            ctr = 0;
        }
        else {
            if (ctr < 255) {
                if (((text[i]>='0') && (text[i]<='9')) ||
                        (text[i]=='.')  || (text[i]==',')) {
                    value_str[ctr++] = text[i];
                }
            }
            else {
                printf("Float array value too big\n");
                break;
            }
        }
    }
}

/* reads an XML file and updates a fingerprint */
int read_fingerprint_xml(stylom_fingerprint * fingerprint, FILE * fp)
{
    char linestr[256];
    int i, j, state = 0, section_ctr=0;
    char section[256], text_body[11*MAX_LETTERS*MAX_LETTERS];
    char text_vocab[26*MAX_VOCAB_SIZE*MAX_WORD_LENGTH];
    int text_body_ctr = 0;
    int active = 0, data_type = 0;
    int retval = 0;

    /* types of data within the xml file */
    enum {
        DATA_NAME = 1,
        DATA_LETTER_HISTOGRAM,
        DATA_LETTER_PAIR_HISTOGRAM,
        DATA_WORD_PAIR_HISTOGRAM,
        DATA_SENTENCE_HISTOGRAM,
        DATA_PARAGRAPH_HISTOGRAM,
        DATA_WORD_HISTOGRAM,
        DATA_DICTIONARY,
        DATA_VOCABULARY,
        DATA_VOCABULARY_FREQUENCY
    };

    while (!feof(fp)) {
        if (fgets(linestr, 255, fp) != NULL ) {
            for (i = 0; i < strlen(linestr); i++) {
                switch (state) {
                case 0: {
                    if (linestr[i] == '<') {
                        state++;
                        section_ctr=0;
                    }
                    break;
                }
                case 1: {
                    if (linestr[i] != '>') {
                        if (section_ctr < 255) {
                            section[section_ctr++] = linestr[i];
                        }
                        else {
                            printf("Section too long\n");
                        }
                    }
                    else {
                        section[section_ctr] = 0;
                        section_ctr = 0;
                        text_body_ctr = 0;
                        state = 0;
                        if ((strcmp(section,"fingerprint")==0) &&
                                (active == 0)) {
                            active = 1;
                        }
                        if ((strcmp(section,"/fingerprint")==0) &&
                                (active == 1)) {
                            active = 0;
                            retval = 1;
                        }
                        if (((strcmp(section,"name")==0) ||
                                (strcmp(section,"Name")==0)) &&
                                (active == 1)) {
                            data_type = DATA_NAME;
                            state = 2;
                        }
                        if (((strcmp(section,"letters")==0) ||
                                (strcmp(section,"Letters")==0)) &&
                                (active == 1)) {
                            data_type = DATA_LETTER_HISTOGRAM;
                            state = 2;
                        }
                        if (((strcmp(section,"letterpairs")==0) ||
                                (strcmp(section,"LetterPairs")==0)) &&
                                (active == 1)) {
                            data_type = DATA_LETTER_PAIR_HISTOGRAM;
                            state = 2;
                        }
                        if (((strcmp(section,"wordpairs")==0) ||
                                (strcmp(section,"WordPairs")==0)) &&
                                (active == 1)) {
                            data_type = DATA_WORD_PAIR_HISTOGRAM;
                            state = 2;
                        }
                        if (((strcmp(section,"sentencelength")==0) ||
                                (strcmp(section,"SentenceLength")==0)) &&
                                (active == 1)) {
                            data_type = DATA_SENTENCE_HISTOGRAM;
                            state = 2;
                        }
                        if (((strcmp(section,"paragraphlength")==0) ||
                                (strcmp(section,"paragraphLength")==0)) &&
                                (active == 1)) {
                            data_type = DATA_PARAGRAPH_HISTOGRAM;
                            state = 2;
                        }
                        if (((strcmp(section,"wordlength")==0) ||
                                (strcmp(section,"WordLength")==0)) &&
                                (active == 1)) {
                            data_type = DATA_WORD_HISTOGRAM;
                            state = 2;
                        }
                        if (((strcmp(section,"dictionary")==0) ||
                                (strcmp(section,"Dictionary")==0)) &&
                                (active == 1)) {
                            data_type = DATA_DICTIONARY;
                            state = 2;
                        }
                        if (((strcmp(section,"vocabulary")==0) ||
                                (strcmp(section,"vocabulary")==0)) &&
                                (active == 1)) {
                            data_type = DATA_VOCABULARY;
                            state = 2;
                        }
                        if (((strcmp(section,"vocabularyfreq")==0) ||
                                (strcmp(section,"vocabularyfreq")==0)) &&
                                (active == 1)) {
                            data_type = DATA_VOCABULARY_FREQUENCY;
                            state = 2;
                        }
                    }
                    break;
                }
                case 2: {
                    if (linestr[i] == '<') {
                        state = 3;
                        section_ctr = 0;
                        text_body[text_body_ctr] = 0;
                    }
                    else {
                        if ((data_type != DATA_VOCABULARY) &&
							(data_type != DATA_VOCABULARY_FREQUENCY)) {
                            if (text_body_ctr <
                                    11*MAX_LETTERS*MAX_LETTERS) {
                                text_body[text_body_ctr++] = linestr[i];
                            }
                            else {
                                printf("text_body too long\n");
                            }
                        }
                        else {
                            if (text_body_ctr <
                                    26*MAX_VOCAB_SIZE*MAX_WORD_LENGTH) {
                                text_vocab[text_body_ctr++] = linestr[i];
                            }
                            else {
                                printf("text_vocab too long\n");
                            }
                        }
                    }
                    break;
                }
                case 3: {
                    if (linestr[i] != '>') {
                        if (section_ctr < 255) {
                            section[section_ctr++] = linestr[i];
                        }
                        else {
                            printf("Final section too long\n");
                        }
                    }
                    else {
                        section[section_ctr] = 0;

                        switch(data_type) {
                        case DATA_NAME: {
                            assert(strlen(text_body)<255);
                            for (j = 0; j < strlen(text_body); j++) {
                                fingerprint->name[j] = text_body[j];
                            }
                            fingerprint->name[j] = 0;
                            break;
                        }
                        case DATA_LETTER_HISTOGRAM: {
                            string_to_float_array(text_body,
                                                  fingerprint->letter_histogram);
                            break;
                        }
                        case DATA_LETTER_PAIR_HISTOGRAM: {
                            string_to_float_array(text_body,
                                                  fingerprint->letter_pair_histogram);
                            break;
                        }
                        case DATA_WORD_PAIR_HISTOGRAM: {
                            string_to_float_array(text_body,
                                                  fingerprint->word_pair_histogram);
                            break;
                        }
                        case DATA_SENTENCE_HISTOGRAM: {
                            string_to_float_array(text_body,
                                                  fingerprint->sentence_length_histogram);
                            break;
                        }
                        case DATA_PARAGRAPH_HISTOGRAM: {
                            string_to_float_array(text_body,
                                                  fingerprint->paragraph_length_histogram);
                            break;
                        }
                        case DATA_WORD_HISTOGRAM: {
                            string_to_float_array(text_body,
                                                  fingerprint->word_length_histogram);
                            break;
                        }
                        case DATA_DICTIONARY: {
                            string_to_float_array(text_body,
                                                  fingerprint->dictionary);
                            break;
                        }
                        case DATA_VOCABULARY: {
                            text_vocab[text_body_ctr] = 0;
                            parse_vocabulary_string(text_vocab,
                                                    fingerprint->vocabulary);
                            break;
                        }
                        case DATA_VOCABULARY_FREQUENCY: {
                            text_vocab[text_body_ctr] = 0;
                            parse_vocabulary_frequency_string(text_vocab,
                                                              fingerprint->vocabulary,
                                                              fingerprint->vocabulary_frequency);
                            break;
                        }
                        }

                        data_type = 0;
                        state = 0;
                    }
                    break;
                }
                }
            }
        }
    }
    return retval;
}

/* save the given fingerprint to an XML file */
void write_fingerprint_xml(stylom_fingerprint * fingerprint, FILE * fp)
{
    int i,ctr=0;
    const int max_cols = 65;

    fprintf(fp,"%s","<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(fp,"%s","<fingerprint>\n");

    fprintf(fp,"  <name>%s</name>\n",fingerprint->name);

    fprintf(fp,"%s","  <letters>\n  ");
    for (i = 0; i < MAX_LETTERS; i++) {
        if (fingerprint->letter_histogram[i]==0) {
            fprintf(fp,"%s ", "0");
            ctr+=2;
        }
        else {
            fprintf(fp,"%.8f ",fingerprint->letter_histogram[i]);
            ctr+=10;
        }
        if (ctr > max_cols) {
            fprintf(fp,"%s","\n  ");
            ctr=0;
        }
    }
    fprintf(fp,"%s","\n  </letters>\n");

    fprintf(fp,"%s","  <letterpairs>\n  ");
    ctr=0;
    for (i = 0; i < MAX_LETTERS*MAX_LETTERS; i++) {
        if (fingerprint->letter_pair_histogram[i]==0) {
            fprintf(fp,"%s ", "0");
            ctr += 2;
        }
        else {
            fprintf(fp,"%.8f ",fingerprint->letter_pair_histogram[i]);
            ctr += 10;
        }
        if (ctr > max_cols) {
            fprintf(fp,"%s","\n  ");
            ctr=0;
        }
    }
    fprintf(fp,"%s","\n  </letterpairs>\n");

    fprintf(fp,"%s","  <wordpairs>\n  ");
    ctr=0;
    for (i = 0; i < MAX_LETTERS*MAX_LETTERS; i++) {
        if (fingerprint->word_pair_histogram[i]==0) {
            fprintf(fp,"%s ", "0");
            ctr += 2;
        }
        else {
            fprintf(fp,"%.8f ",fingerprint->word_pair_histogram[i]);
            ctr += 10;
        }
        if (ctr > max_cols) {
            fprintf(fp,"%s","\n  ");
            ctr=0;
        }
    }
    fprintf(fp,"%s","\n  </wordpairs>\n");

    fprintf(fp,"%s","  <sentencelength>\n  ");
    ctr=0;
    for (i = 0; i < MAX_WORDS_PER_SENTENCE; i++) {
        if (fingerprint->sentence_length_histogram[i]==0) {
            fprintf(fp,"%s ", "0");
            ctr += 2;
        }
        else {
            fprintf(fp,"%.8f ",fingerprint->sentence_length_histogram[i]);
            ctr += 10;
        }
        if (ctr > max_cols) {
            fprintf(fp,"%s","\n  ");
            ctr=0;
        }
    }
    fprintf(fp,"%s","\n  </sentencelength>\n");

    fprintf(fp,"%s","  <paragraphlength>\n  ");
    ctr=0;
    for (i = 0; i < MAX_SENTENCES_PER_PARAGRAPH; i++) {
        if (fingerprint->paragraph_length_histogram[i]==0) {
            fprintf(fp,"%s ", "0");
            ctr += 2;
        }
        else {
            fprintf(fp,"%.8f ",fingerprint->paragraph_length_histogram[i]);
            ctr += 10;
        }
        if (ctr > max_cols) {
            fprintf(fp,"%s","\n  ");
            ctr=0;
        }
    }
    fprintf(fp,"%s","\n  </paragraphlength>\n");

    fprintf(fp,"%s","  <wordlength>\n  ");
    ctr=0;
    for (i = 0; i < MAX_WORD_LENGTH; i++) {
        if (fingerprint->word_length_histogram[i]==0) {
            fprintf(fp,"%s ", "0");
            ctr += 2;
        }
        else {
            fprintf(fp,"%.8f ",fingerprint->word_length_histogram[i]);
            ctr += 10;
        }
        if (ctr > max_cols) {
            fprintf(fp,"%s","\n  ");
            ctr=0;
        }
    }
    fprintf(fp,"%s","\n  </wordlength>\n");

    fprintf(fp,"%s","  <dictionary>\n  ");
    ctr=0;
    for (i = 0; i < DICTIONARY_SIZE; i++) {
        if (fingerprint->dictionary[i]==0) {
            fprintf(fp,"%s ", "0");
            ctr += 2;
        }
        else {
            fprintf(fp,"%.8f ",fingerprint->dictionary[i]);
            ctr += 10;
        }
        if (ctr > max_cols) {
            fprintf(fp,"%s","\n  ");
            ctr=0;
        }
    }
    fprintf(fp,"%s","\n  </dictionary>\n");

    write_vocabulary_xml(fingerprint->vocabulary,
                         fingerprint->vocabulary_frequency,
                         fp);

    fprintf(fp,"%s","</fingerprint>\n");
}

int fingerprints_identical(stylom_fingerprint * f1,
                           stylom_fingerprint * f2,
                           int include_vocabulary)
{
    int i,j;
    float diff;
    const float max_diff = 0.00001f;

    if (strcmp(f1->name,f2->name)!=0) {
        return -1;
    }

    for (i = 0; i < MAX_LETTERS; i++) {
        diff = fabs(f1->letter_histogram[i] -
                    f2->letter_histogram[i]);
        if (diff > max_diff) {
            return -2;
        }
    }

    for (i = 0; i < MAX_LETTERS*MAX_LETTERS; i++) {
        diff = fabs(f1->letter_pair_histogram[i] -
                    f2->letter_pair_histogram[i]);
        if (diff > max_diff) {
            return -3;
        }
        diff = fabs(f1->word_pair_histogram[i] -
                    f2->word_pair_histogram[i]);
        if (diff > max_diff) {
            return -4;
        }
    }

    for (i = 0; i < MAX_WORDS_PER_SENTENCE; i++) {
        diff = fabs(f1->sentence_length_histogram[i] -
                    f2->sentence_length_histogram[i]);
        if (diff > max_diff) {
            return -5;
        }
    }

    for (i = 0; i < MAX_SENTENCES_PER_PARAGRAPH; i++) {
        diff = fabs(f1->paragraph_length_histogram[i] -
                    f2->paragraph_length_histogram[i]);
        if (diff > max_diff) {
            return -6;
        }
    }

    for (i = 0; i < MAX_WORD_LENGTH; i++) {
        diff = fabs(f1->word_length_histogram[i] -
                    f2->word_length_histogram[i]);
        if (diff > max_diff) {
            return -7;
        }
    }
    for (i = 0; i < DICTIONARY_SIZE; i++) {
        diff = fabs(f1->dictionary[i] -
                    f2->dictionary[i]);
        if (diff > max_diff) {
            return -8;
        }
    }
    if (include_vocabulary > 0) {
        for (i = 0; i < 26; i++) {
            for (j = 0; j < MAX_VOCAB_SIZE; j++) {
                if (strcmp(&f1->vocabulary[VINDEX(i,j)],
                           &f2->vocabulary[VINDEX(i,j)])!=0) {
                    return -9;
                }
                diff = fabs(f1->vocabulary_frequency[FINDEX(i,j)] -
                            f2->vocabulary_frequency[FINDEX(i,j)]);
                if (diff > max_diff) {
                    return -10;
                }
            }
        }
    }
    return 0;
}

/* clears the given fingerprint */
static void fingerprint_clear(stylom_fingerprint * f)
{
    f->name[0] = 0;

    memset((void*)f->letter_histogram,'\0',
           MAX_LETTERS*sizeof(float));

    memset((void*)f->letter_pair_histogram,'\0',
           MAX_LETTERS*MAX_LETTERS*sizeof(float));

    memset((void*)f->word_pair_histogram,'\0',
           MAX_LETTERS*MAX_LETTERS*sizeof(float));

    memset((void*)f->sentence_length_histogram,'\0',
           MAX_WORDS_PER_SENTENCE*sizeof(float));

    memset((void*)f->paragraph_length_histogram,'\0',
           MAX_SENTENCES_PER_PARAGRAPH*sizeof(float));

    memset((void*)f->word_length_histogram,'\0',
           MAX_WORD_LENGTH*sizeof(float));

    memset((void*)f->dictionary,'\0',
           DICTIONARY_SIZE*sizeof(float));
}

/* adds one fingerprint to another */
static void fingerprint_sum(stylom_fingerprint * source,
                            stylom_fingerprint * dest)
{
    int i;

    for (i = 0; i < MAX_LETTERS; i++) {
        dest->letter_histogram[i] +=
            source->letter_histogram[i];
    }

    for (i = 0; i < MAX_LETTERS*MAX_LETTERS; i++) {
        dest->letter_pair_histogram[i] +=
            source->letter_pair_histogram[i];

        dest->word_pair_histogram[i] +=
            source->word_pair_histogram[i];
    }

    for (i = 0; i < MAX_WORDS_PER_SENTENCE; i++) {
        dest->sentence_length_histogram[i] +=
            source->sentence_length_histogram[i];
    }

    for (i = 0; i < MAX_SENTENCES_PER_PARAGRAPH; i++) {
        dest->paragraph_length_histogram[i] +=
            source->paragraph_length_histogram[i];
    }

    for (i = 0; i < MAX_WORD_LENGTH; i++) {
        dest->word_length_histogram[i] +=
            source->word_length_histogram[i];
    }

    for (i = 0; i < DICTIONARY_SIZE; i++) {
        dest->dictionary[i] +=
            source->dictionary[i];
    }
}

static void fingerprint_subtract(stylom_fingerprint * a,
                                 stylom_fingerprint * b)
{
    int i;

    for (i = 0; i < MAX_LETTERS; i++) {
        a->letter_histogram[i] -=
            b->letter_histogram[i];
    }

    for (i = 0; i < MAX_LETTERS*MAX_LETTERS; i++) {
        a->letter_pair_histogram[i] -=
            b->letter_pair_histogram[i];

        a->word_pair_histogram[i] -=
            b->word_pair_histogram[i];
    }

    for (i = 0; i < MAX_WORDS_PER_SENTENCE; i++) {
        a->sentence_length_histogram[i] -=
            b->sentence_length_histogram[i];
    }

    for (i = 0; i < MAX_SENTENCES_PER_PARAGRAPH; i++) {
        a->paragraph_length_histogram[i] -=
            b->paragraph_length_histogram[i];
    }

    for (i = 0; i < MAX_WORD_LENGTH; i++) {
        a->word_length_histogram[i] -=
            b->word_length_histogram[i];
    }

    for (i = 0; i < DICTIONARY_SIZE; i++) {
        a->dictionary[i] -=
            b->dictionary[i];
    }
}

/* returns a value indicating the similarity between two fingerprints */
float fingerprint_similarity(stylom_fingerprint * a, stylom_fingerprint * b)
{
    stylom_fingerprint diff;
    float dist[6],v;
    int i;

    /* calculate the difference between the two */
    memcpy((void*)(&diff), (void*)a, sizeof(stylom_fingerprint));
    fingerprint_subtract(&diff, b);

    /* square the differences */
    dist[0] = 0;
    for (i = 0; i < MAX_LETTERS; i++) {
        dist[0] += diff.letter_histogram[i] * diff.letter_histogram[i];
    }
    dist[0] /= MAX_LETTERS;

    dist[1] = 0;
    for (i = 0; i < MAX_LETTERS*MAX_LETTERS; i++) {
        dist[1] += diff.letter_pair_histogram[i] * diff.letter_pair_histogram[i];
    }
    dist[1] /= MAX_LETTERS*MAX_LETTERS;

    dist[2] = 0;
    for (i = 0; i < MAX_LETTERS*MAX_LETTERS; i++) {
        dist[2] += diff.word_pair_histogram[i] * diff.word_pair_histogram[i];
    }
    dist[2] /= MAX_LETTERS*MAX_LETTERS;

    dist[3] = 0;
    for (i = 0; i < MAX_WORDS_PER_SENTENCE; i++) {
        dist[3] +=
            diff.sentence_length_histogram[i] *
            diff.sentence_length_histogram[i];
    }
    dist[3] /= MAX_WORDS_PER_SENTENCE;

    dist[4] = 0;
    for (i = 0; i < MAX_SENTENCES_PER_PARAGRAPH; i++) {
        dist[4] +=
            diff.paragraph_length_histogram[i] *
            diff.paragraph_length_histogram[i];
    }
    dist[4] /= MAX_SENTENCES_PER_PARAGRAPH;

    dist[5] = 0;
    for (i = 0; i < MAX_WORD_LENGTH; i++) {
        dist[5] += diff.word_length_histogram[i] *
                   diff.word_length_histogram[i];
    }
    dist[5] /= MAX_WORD_LENGTH;

    dist[6] = 0;
    for (i = 0; i < DICTIONARY_SIZE; i++) {
        dist[6] += diff.dictionary[i] *
                   diff.dictionary[i];
    }
    dist[6] /= DICTIONARY_SIZE;

    v = (float)sqrt((dist[0] + dist[1] + dist[2] +
                     dist[3] + dist[4] + dist[5] +
					 dist[6])/7.0f);
    return 100 - (v*100);
}

/* Divide the fingerprint values by a given number.
   This is used to calculate averages */
static void fingerprint_divide(stylom_fingerprint * f, int divisor)
{
    int i;

    for (i = 0; i < MAX_LETTERS; i++) {
        f->letter_histogram[i] /= divisor;
    }

    for (i = 0; i < MAX_LETTERS*MAX_LETTERS; i++) {
        f->letter_pair_histogram[i] /= divisor;
        f->word_pair_histogram[i] /= divisor;
    }

    for (i = 0; i < MAX_WORDS_PER_SENTENCE; i++) {
        f->sentence_length_histogram[i] /= divisor;
    }

    for (i = 0; i < MAX_SENTENCES_PER_PARAGRAPH; i++) {
        f->paragraph_length_histogram[i] /= divisor;
    }

    for (i = 0; i < MAX_WORD_LENGTH; i++) {
        f->word_length_histogram[i] /= divisor;
    }

    for (i = 0; i < DICTIONARY_SIZE; i++) {
        f->dictionary[i] /= divisor;
    }
}

/* Returns an average model for fingerprints in the given directory.
   This can be used to calculate eigenvectors */
int fingerprint_average(stylom_fingerprint * f,
                        char * directory)
{
    DIR * dp;
    stylom_fingerprint fingerprint;
    struct dirent *ep;
    char filename[256];
    FILE * fp;
    int instances = 0;

    fingerprint_clear(f);

    dp = opendir (directory);
    if (dp != NULL) {
        while ((ep = readdir (dp)) != NULL) {
            if ((strcmp(ep->d_name,".")!=0) &&
                    (strcmp(ep->d_name,"..")!=0)) {
                sprintf(filename,"%s/%s",directory,ep->d_name);
                fp = fopen(filename,"r");
                if (fp) {
                    if (read_fingerprint_xml(&fingerprint, fp)!=0) {
                        fingerprint_sum(&fingerprint, f);
                        instances++;
                    }
                    fclose(fp);
                }
            }
        }
        (void) closedir (dp);

        if (instances>0) {
            fingerprint_divide(f, instances);
        }
        return 1;
    }
    return 0;
}

static void fingerprint_eigenvector(stylom_fingerprint * f,
                                    stylom_fingerprint * average,
                                    stylom_fingerprint * eigen)
{
    /* copy the eigenvector */
    memcpy((void*)eigen,(void*)f,sizeof(stylom_fingerprint));
    /* subtract the average */
    fingerprint_subtract(eigen, average);
}

/* matches the given fingerprint against those in the given directory
   and returns the name of the best match */
int fingerprint_match(stylom_fingerprint * f,
                      char * directory,
                      char * best_name,
                      int show_list)
{
    stylom_fingerprint fingerprint, fingerprint_av;
    stylom_fingerprint fingerprint_eigen, f_eigen;
    DIR * dp;
    struct dirent *ep;
    char filename[256];
    FILE * fp;
    float max_similarity=0,similarity;
    int min_index, max_index, i, j, retval = 0, list_index = 0;
    char * list_name[STYLOM_LIST_LENGTH], * temp_name;
    float list_similarity[STYLOM_LIST_LENGTH];
    float min_similarity, temp_similarity;

    /* clear the best name */
    best_name[0]=0;

    /* get the average of all fingerprints */
    fingerprint_average(&fingerprint_av, directory);
    /* calculate the eigenvector */
    fingerprint_eigenvector(f,&fingerprint_av,&f_eigen);

    dp = opendir (directory);
    if (dp != NULL) {
        /* for every file in the directory */
        while ((ep = readdir (dp)) != NULL) {
            if ((strcmp(ep->d_name,".")!=0) &&
                    (strcmp(ep->d_name,"..")!=0)) {
                /* the full filename */
                sprintf(filename,"%s/%s",directory,ep->d_name);
                /* open the file */
                fp = fopen(filename,"r");
                if (fp) {
                    /* read the fingerprint */
                    if (read_fingerprint_xml(&fingerprint, fp)!=0) {
                        /* calculate eigenvector */
                        fingerprint_eigenvector(&fingerprint,
												&fingerprint_av,
                                                &fingerprint_eigen);
                        /* similarity of this fingerprint eigenvector to
                           the eigenvector of the one given */
                        similarity =
                            fingerprint_similarity(&f_eigen,
                                                   &fingerprint_eigen);

                        if (show_list!=0) {
                            min_similarity = 9999;
                            min_index = 0;
                            for (i = 0; i < list_index; i++) {
                                if (list_similarity[i] <
									min_similarity) {
                                    min_similarity = list_similarity[i];
                                    min_index = i;
                                }
                            }
                            if ((similarity > min_similarity) ||
                                    (list_index < STYLOM_LIST_LENGTH)) {
                                if (list_index < STYLOM_LIST_LENGTH) {
                                    list_similarity[list_index] =
										similarity;
                                    list_name[list_index] =
                                        (char*)malloc(256*sizeof(char));
                                    if (fingerprint.name[0]!=0) {
                                        sprintf(list_name[list_index],
												"%s",
                                                (&fingerprint)->name);
                                    }
                                    else {
                                        sprintf(list_name[list_index],
                                                "%s",ep->d_name);
                                    }
                                    list_index++;
                                }
                                else {
                                    list_similarity[min_index] =
										similarity;
                                    if (fingerprint.name[0]!=0) {
                                        sprintf(list_name[min_index],
												"%s",
                                                (&fingerprint)->name);
                                    }
                                    else {
                                        sprintf(list_name[min_index],
                                                "%s",ep->d_name);
                                    }
                                }
                            }
                        }

                        /* is this the best? */
                        if (similarity > max_similarity) {
                            max_similarity = similarity;
                            if (fingerprint.name[0]!=0) {
                                /* if an author name is specified */
                                sprintf(best_name,"%s",
										(&fingerprint)->name);
                            }
                            else {
                                /* if no name was assigned */
                                sprintf(best_name,"%s",ep->d_name);
                            }
                            retval = 1;
                        }
                    }
                    fclose(fp);
                }
            }
        }
        (void) closedir (dp);

        if (list_index > 0) {
            /* sort the list */
            for (i = 0; i < list_index-1; i++) {
                max_similarity = list_similarity[i];
                max_index = i;
                for (j = i+1; j < list_index; j++) {
                    if (list_similarity[j] > max_similarity) {
                        max_similarity = list_similarity[j];
                        max_index = j;
                    }
                }
                if (max_index != i) {
                    temp_similarity = list_similarity[i];
                    list_similarity[i] = list_similarity[max_index];
                    list_similarity[max_index] = temp_similarity;
                    temp_name = list_name[i];
                    list_name[i] = list_name[max_index];
                    list_name[max_index] = temp_name;
                }
            }
            /* show the list */
            for (i = 0; i < list_index; i++) {
                printf("%.8f\t%s\n",list_similarity[i],list_name[i]);
                free(list_name[i]);
            }
        }
    }

    return retval;
}

/* Calculates 2D coordinates for a fingerprint relative to fingerprints within
   a given directory */
void  fingerprint_coordinates(stylom_fingerprint * f, float * x, float * y)
{
    int i;

    *x = 0;
    for (i = 0; i < MAX_LETTERS; i++) {
        (*x) = (*x) + (f->letter_histogram[i]);
    }

    for (i = 0; i < MAX_LETTERS*MAX_LETTERS; i++) {
        (*x) = (*x) + (f->letter_pair_histogram[i]);
        (*x) = (*x) + (f->word_pair_histogram[i]);
    }

    for (i = 0; i < MAX_SENTENCES_PER_PARAGRAPH; i++) {
        (*x) = (*x) + (f->paragraph_length_histogram[i]);
    }
    (*x) = (*x) / (MAX_LETTERS + (MAX_LETTERS*MAX_LETTERS*2) +
				   MAX_SENTENCES_PER_PARAGRAPH);

    *y = 0;
    for (i = 0; i < MAX_WORDS_PER_SENTENCE; i++) {
        (*y) = (*y) + (f->sentence_length_histogram[i]);
    }

    for (i = 0; i < MAX_WORD_LENGTH; i++) {
        (*y) = (*y) + (f->word_length_histogram[i]);
    }
    for (i = 0; i < DICTIONARY_SIZE; i++) {
        (*y) = (*y) + (f->dictionary[i]);
    }
    (*y) = (*y) / (MAX_WORDS_PER_SENTENCE +
                   MAX_WORD_LENGTH + DICTIONARY_SIZE);
}

static void fingerprints_range(char * directory,
                               float * min_x, float * min_y,
                               float * max_x, float * max_y)
{
    char filename[256];
    FILE * fp;
    DIR * dp;
    struct dirent *ep;
    stylom_fingerprint fingerprint, fingerprint_av;
    stylom_fingerprint fingerprint_eigen;
    float x, y;

    *min_x = 9999;
    *min_y = 9999;
    *max_x = -9999;
    *max_y = -9999;

    /* get the average of all fingerprints */
    fingerprint_average(&fingerprint_av, directory);

    dp = opendir (directory);
    if (dp != NULL) {
        /* for every file in the directory */
        while ((ep = readdir (dp)) != NULL) {
            if ((strcmp(ep->d_name,".")!=0) &&
                    (strcmp(ep->d_name,"..")!=0)) {
                /* the full filename */
                sprintf(filename,"%s/%s",directory,ep->d_name);
                /* open the file */
                fp = fopen(filename,"r");
                if (fp) {
                    /* read the fingerprint */
                    if (read_fingerprint_xml(&fingerprint,
                                             fp)!=0) {
                        /* calculate eigenvector */
                        fingerprint_eigenvector(&fingerprint,
                                                &fingerprint_av,
                                                &fingerprint_eigen);
                        /* coordinates of the eigenvector */
                        fingerprint_coordinates(&fingerprint_eigen,
                                                &x, &y);

                        if (x < (*min_x)) (*min_x) = x;
                        if (y < (*min_y)) (*min_y) = y;
                        if (x > (*max_x)) (*max_x) = x;
                        if (y > (*max_y)) (*max_y) = y;
                    }
                    fclose(fp);
                }
            }
        }
        (void) closedir (dp);
    }
}

/* plot fingerprints within the given directory as points */
int plot_fingerprints(char * image_filename,
                      char * title, char * directory,
                      int width, int height)
{
    char * data_filename = "/tmp/temp_data.dat";
    char * plot_filename = "/tmp/temp_plotfile.plot";
    char commandstr[256],filename[256];
    FILE * fp, * fp_data;
    int retval=0;
    DIR * dp;
    struct dirent *ep;
    stylom_fingerprint fingerprint, fingerprint_av;
    stylom_fingerprint fingerprint_eigen;
    float x, y;
    float min_x = 9999, min_y = 9999;
    float max_x = -9999, max_y = -9999;

    fp = fopen(plot_filename,"r");
    if (fp) {
        fclose(fp);
        sprintf((char*)commandstr,"rm %s", plot_filename);
        retval = system(commandstr);
    }

    /* get the minimum and maximum range */
    fingerprints_range(directory, &min_x, &min_y,
                       &max_x, &max_y);

    /* get the average of all fingerprints */
    fingerprint_average(&fingerprint_av, directory);

    fp_data = fopen(data_filename,"w");
    if (!fp_data) return 0;

    dp = opendir (directory);
    if (dp != NULL) {
        /* for every file in the directory */
        while ((ep = readdir (dp)) != NULL) {
            if ((strcmp(ep->d_name,".")!=0) &&
                    (strcmp(ep->d_name,"..")!=0)) {
                /* the full filename */
                sprintf(filename,"%s/%s",directory,ep->d_name);
                /* open the file */
                fp = fopen(filename,"r");
                if (fp) {
                    /* read the fingerprint */
                    if (read_fingerprint_xml(&fingerprint,
                                             fp)!=0) {
                        fclose(fp);
                        /* calculate eigenvector */
                        fingerprint_eigenvector(&fingerprint,
                                                &fingerprint_av,
                                                &fingerprint_eigen);
                        /* coordinates of the eigenvector */
                        fingerprint_coordinates(&fingerprint_eigen,
                                                &x, &y);

                        x = 5 + ((x - min_x)*90/(max_x - min_x));
                        y = 5 + ((y - min_y)*90/(max_y - min_y));

                        if (fingerprint.name[0]!=0) {
                            fprintf(fp_data,
                                    "%.8f %.8f \"%s\"\n",
                                    x, y, fingerprint.name);
                        }
                        else {
                            fprintf(fp_data,
                                    "%.8f %.8f \"%s\"\n",
                                    x, y, ep->d_name);
                        }
                    }
                    else {
                        fclose(fp);
                    }
                }
            }
        }
        (void) closedir (dp);
    }
    fclose(fp_data);

    fp = fopen(plot_filename,"w");
    if (fp) {
        fprintf(fp,"reset\n");
        fprintf(fp,"set title \"%s\"\n",title);
        fprintf(fp,"set lmargin 9\n");
        fprintf(fp,"set rmargin 2\n");
        fprintf(fp,"set xlabel \"X\"\n");
        fprintf(fp,"set ylabel \"Y\"\n");
        fprintf(fp,"set xrange [0:100]\n");
        fprintf(fp,"set yrange [0:100]\n");
        fprintf(fp,"set key off\n");
        fprintf(fp,"set terminal png size %d,%d ",
                width,height);
        fprintf(fp,"%s","font courier 10\n");
        fprintf(fp,"set output \"%s\"\n",image_filename);
        fprintf(fp,"plot \"%s\" using 1:2 with points\n",
                data_filename);

        fclose(fp);

        sprintf((char*)commandstr,"gnuplot %s",plot_filename);
        retval=system(commandstr);
    }

    return retval;
}

/* compares the given text to a fingerprint of an existing author */
int fingerprint_compare(char * text,
                        char * author_fingerprint_filename)
{
    FILE * fp;
    stylom_fingerprint author_fingerprint;
    stylom_fingerprint text_fingerprint;
    char more[1024],less[1024],missing[1024];
    int no_of_differences, sentence_start=0;
	int sentence_end=0, add_words=0,sentence_line_number,i;

    fp = fopen(author_fingerprint_filename,"r");
    if (!fp) return -1;

    /* load the author fingerprint */
    if (read_fingerprint_xml(&author_fingerprint, fp)==1) {
        fclose(fp);
        /* calculate the text fingerprint */
        get_fingerprint("Unknown", text, &text_fingerprint);
        no_of_differences =
            compare_vocabulary((&text_fingerprint)->vocabulary,
                               (&author_fingerprint)->vocabulary,
                               (&text_fingerprint)->vocabulary_frequency,
                               (&author_fingerprint)->vocabulary_frequency,
                               missing, more, less,
                               SIMILARITY_THRESHOLD,
                               CONSOLE_WIDTH-20);

		sentence_line_number =
			recommend_sentence_change(text,
									  &text_fingerprint,
									  &author_fingerprint,
									  &sentence_start,
									  &sentence_end,
									  &add_words);

        printf("\nNumber of style differences: %d\n\n",
               no_of_differences);
        if (strlen(missing)>0) {
            printf("Avoid using the following words:\n\n");
            printf("%s\n\n", missing);
        }
        if (strlen(more)>0) {
            printf("Use more of the following words:\n\n");
            printf("%s\n\n", more);
        }
        if (strlen(less)>0) {
            printf("Use fewer of the following words:\n\n");
            printf("%s\n\n", less);
        }
		if (sentence_line_number > -1) {
			if (add_words > 0) {
				printf("Add %d words\n",add_words);
			}
			else {
				printf("Remove %d words\n",-add_words);
			}
			printf("Line number %d\n",sentence_line_number);
			for (i = sentence_start; i < sentence_end; i++) {
				if ((text[i] != 13) && (text[i] != 10)) {
					printf("%c",text[i]);
				}
				else {
					if ((i > 0) && (text[i] != 13)) {
						if (text[i-1]!=' ') {
							printf(" ");
						}
					}
				}
			}
			printf("\n");
		}
    }
    else {
        fclose(fp);
        printf("Cound not open %s\n",author_fingerprint_filename);
    }
    return 0;
}

/* reports words which exist in the given text but which are missing
   from the given fingerprint */
int fingerprint_missing_words(char * text,
                              char * author_fingerprint_filename)
{
    FILE * fp;
    stylom_fingerprint author_fingerprint;
    stylom_fingerprint text_fingerprint;
    char more[1024],less[1024],missing[1024];

    fp = fopen(author_fingerprint_filename,"r");
    if (!fp) return -1;

    /* load the author fingerprint */
    if (read_fingerprint_xml(&author_fingerprint, fp)==1) {
        fclose(fp);
        /* calculate the text fingerprint */
        get_fingerprint("Unknown", text, &text_fingerprint);
        compare_vocabulary((&text_fingerprint)->vocabulary,
                           (&author_fingerprint)->vocabulary,
                           (&text_fingerprint)->vocabulary_frequency,
                           (&author_fingerprint)->vocabulary_frequency,
                           missing, more, less,
                           SIMILARITY_THRESHOLD, 9999);
        if (strlen(missing)>0) {
            printf("%s\n", missing);
        }
    }
    else {
        fclose(fp);
        printf("Cound not open %s\n",author_fingerprint_filename);
    }
    return 0;
}

/* reports words which are more frequent in the given fingerprint
   than in the given text */
int fingerprint_more_frequent_words(char * text,
                                    char * author_fingerprint_filename)
{
    FILE * fp;
    stylom_fingerprint author_fingerprint;
    stylom_fingerprint text_fingerprint;
    char more[1024],less[1024],missing[1024];

    fp = fopen(author_fingerprint_filename,"r");
    if (!fp) return -1;

    /* load the author fingerprint */
    if (read_fingerprint_xml(&author_fingerprint, fp)==1) {
        fclose(fp);
        /* calculate the text fingerprint */
        get_fingerprint("Unknown", text, &text_fingerprint);
        compare_vocabulary((&text_fingerprint)->vocabulary,
                           (&author_fingerprint)->vocabulary,
                           (&text_fingerprint)->vocabulary_frequency,
                           (&author_fingerprint)->vocabulary_frequency,
                           missing, more, less,
                           SIMILARITY_THRESHOLD, 9999);
        if (strlen(more)>0) {
            printf("%s\n", more);
        }
    }
    else {
        fclose(fp);
        printf("Cound not open %s\n",author_fingerprint_filename);
    }
    return 0;
}

/* reports words which are less frequent in the given fingerprint
   than in the given text */
int fingerprint_less_frequent_words(char * text,
                                    char * author_fingerprint_filename)
{
    FILE * fp;
    stylom_fingerprint author_fingerprint;
    stylom_fingerprint text_fingerprint;
    char more[1024],less[1024],missing[1024];

    fp = fopen(author_fingerprint_filename,"r");
    if (!fp) return -1;

    /* load the author fingerprint */
    if (read_fingerprint_xml(&author_fingerprint, fp)==1) {
        fclose(fp);
        /* calculate the text fingerprint */
        get_fingerprint("Unknown", text, &text_fingerprint);
        compare_vocabulary((&text_fingerprint)->vocabulary,
                           (&author_fingerprint)->vocabulary,
                           (&text_fingerprint)->vocabulary_frequency,
                           (&author_fingerprint)->vocabulary_frequency,
                           missing, more, less,
                           SIMILARITY_THRESHOLD, 9999);
        if (strlen(less)>0) {
            printf("%s\n", less);
        }
    }
    else {
        fclose(fp);
        printf("Cound not open %s\n",author_fingerprint_filename);
    }
    return 0;
}

/* plot texts within a directory */
int plot_texts(char * image_filename,
               char * title, char * directory,
               int width, int height)
{
    DIR * dp;
    struct dirent *ep;
    char temp_directory[256],text_filename[256];
    char fingerprint_filename[256], commandstr[256];
    char * text;
    FILE * fp;
    stylom_fingerprint fingerprint;
    int result;

    sprintf(temp_directory,"%s","/tmp/temp_stylom_plot_texts");

    /* delete any previous temporary ditrectory */
    sprintf(commandstr,"rm -rf %s",temp_directory);
    result = system(commandstr);

    /* create the temporary directory */
    sprintf(commandstr,"mkdir %s",temp_directory);
    if (system(commandstr)!=0) {
        printf("Unable to create temporary directory %s\n",
               temp_directory);
    }

    dp = opendir(directory);
    if (dp != NULL) {
        /* for every file in the directory */
        while ((ep = readdir (dp)) != NULL) {
            if ((strcmp(ep->d_name,".")!=0) &&
                    (strcmp(ep->d_name,"..")!=0)) {
                /* the full filename */
                sprintf(text_filename,"%s/%s",directory,ep->d_name);
                printf("%s\n",text_filename);
                text = 0;
                if (read_text_from_file(&text, text_filename)!=0) {
                    /* calculate the fingerprint */
                    get_fingerprint(ep->d_name, text, &fingerprint);
                    /* save the fingerprint in the temporary directory */
                    sprintf(fingerprint_filename,
                            "%s/%s", temp_directory, ep->d_name);
                    fp = fopen(fingerprint_filename,"w");
                    if (fp) {
                        write_fingerprint_xml(&fingerprint, fp);
                        fclose(fp);
                    }
                }
                if (text != 0) {
                    free(text);
                }
            }
        }
        (void)closedir(dp);
    }

    result = plot_fingerprints(image_filename, title, temp_directory,
                               width, height);

    return result;
}
