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

#ifndef STYLOM_H
#define STYLOM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <regex.h>
#include <errno.h>
#include <err.h>
#include <assert.h>
#include <sys/time.h>
#include <time.h>
#include <ctype.h>

#define VERSION 1.00

#define CONSOLE_WIDTH                80

#define MAX_LETTERS                 128
#define MAX_WORDS_PER_SENTENCE      256
#define MAX_SENTENCES_PER_PARAGRAPH 128
#define MAX_WORD_LENGTH              32
#define STYLOM_LIST_LENGTH           10
#define DICTIONARY_SIZE             (MAX_LETTERS*MAX_LETTERS)
#define MAX_COMPARE_RESULTS          32
#define SIMILARITY_THRESHOLD        0.01

#define MAX_VOCAB_SIZE              640

#define VINDEX(i,j) (((i)*MAX_VOCAB_SIZE*MAX_WORD_LENGTH) + \
					 ((j)*MAX_WORD_LENGTH))
#define FINDEX(i,j) (((i)*MAX_VOCAB_SIZE)+(j))


struct stylom_fing {
    char name[256];
    float letter_histogram[MAX_LETTERS];
    float letter_pair_histogram[MAX_LETTERS*MAX_LETTERS];
    float word_pair_histogram[MAX_LETTERS*MAX_LETTERS];
    float sentence_length_histogram[MAX_WORDS_PER_SENTENCE];
    float paragraph_length_histogram[MAX_SENTENCES_PER_PARAGRAPH];
    float word_length_histogram[MAX_WORD_LENGTH];
    float dictionary[DICTIONARY_SIZE];
    char vocabulary[26*MAX_VOCAB_SIZE*MAX_WORD_LENGTH];
    float vocabulary_frequency[26*MAX_VOCAB_SIZE];
};
typedef struct stylom_fing stylom_fingerprint;

int read_text_from_file(char ** text, char * filename);
int word_terminator(char ch1, char ch2);
float word_to_float(char * text, int start_pos, int end_pos, float mult);
void get_fingerprint(char * name, char * text,
					 stylom_fingerprint * fingerprint);
void write_fingerprint_xml(stylom_fingerprint * fingerprint, FILE * fp);
int read_fingerprint_xml(stylom_fingerprint * fingerprint, FILE * fp);
int fingerprints_identical(stylom_fingerprint * f1,
                           stylom_fingerprint * f2,
                           int include_vocabulary);
int fingerprint_average(stylom_fingerprint * f, char * directory);
float fingerprint_similarity(stylom_fingerprint * a,
							 stylom_fingerprint * b);
int fingerprint_match(stylom_fingerprint * f, char * directory,
                      char * best_name, int show_list);
void  fingerprint_coordinates(stylom_fingerprint * f,
							  float * x, float * y);
int plot_fingerprints(char * image_filename,
                      char * title, char * directory,
                      int width, int height);
int plot_texts(char * image_filename,
               char * title, char * directory,
               int width, int height);
int create_dictionary(char * text, float dictionary[]);
int create_vocabulary(char * text, char * vocab, float * frequency);
void write_vocabulary_xml(char * vocab, float * frequency, FILE * fp);
void parse_vocabulary_string(char * text, char* vocab);
void parse_vocabulary_frequency_string(char * text,
                                       char* vocab,
                                       float * frequency);
int fingerprint_compare(char * text,
                        char * author_fingerprint_filename);
int compare_vocabulary(char * vocab1, char * vocab2,
                       float * frequency1, float * frequency2,
                       char * missing,
                       char * more, char * less,
                       float threshold,
                       int line_length);
int fingerprint_less_frequent_words(char * text,
                                    char * author_fingerprint_filename);
int fingerprint_more_frequent_words(char * text,
                                    char * author_fingerprint_filename);
int fingerprint_missing_words(char * text,
                              char * author_fingerprint_filename);

#endif
