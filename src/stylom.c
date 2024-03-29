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

enum {
    FUNCTION_NONE = 0,
    FUNCTION_FINGERPRINT,
    FUNCTION_MATCH,
    FUNCTION_PLOT_FINGERPRINTS,
    FUNCTION_PLOT_TEXTS,
    FUNCTION_COMPARE,
    FUNCTION_MISSING,
    FUNCTION_MORE,
    FUNCTION_LESS
};

void show_help()
{
    printf("Stylom: A command line stylometry utility\n\n");
    printf(" -t  --text <string>         Text to be analysed\n");
    printf(" -n  --name <string>         Name of the author\n");
    printf(" -f  --fingerprint           Show fingerprint\n");
    printf(" -m  --match <directory>     Match the current text against ");
    printf("fingerprints\n");
    printf("                             in the given directory\n");
    printf(" -l  --list <directory>      Shows best matches list\n");
    printf(" -c  --compare <fingerprint> Compare text against the ");
    printf("given fingerprint\n");
    printf("     --missing <fingerprint> Report words which are ");
    printf("present in the\n");
    printf("                             current text but which are missing\n");
    printf("                             from the given fingerprint\n");
    printf("     --more <fingerprint>    Report words which are ");
    printf("present more often in the\n");
    printf("                             given fingerprint than ");
    printf("in the current text\n");
    printf("     --less <fingerprint>    Report words which are ");
    printf("present less often in the\n");
    printf("                             given fingerprint than ");
    printf("in the current text\n");
    printf(" -p  --plot <directory>      Plot fingerprints within the ");
    printf("given directory\n");
    printf("     --plottexts <directory> Plot texts within the ");
    printf("given directory\n");
    printf("     --title <text>          Plot title\n");
    printf(" -v  --version               Show the version number\n");
    printf(" -h  --help                  Show help\n");
}

/* returns true if there is data waiting within stdin */
int stdin_has_data(int wait_sec)
{
    fd_set rfds;
    struct timeval tv;

    /* Watch stdin (fd 0) to see when it has input. */
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);

    /* Wait */
    tv.tv_sec = wait_sec;
    tv.tv_usec = 0;

    /* does data exist within stdin? */
    return select(1, &rfds, NULL, NULL, &tv);
}

/* reads text from stdin */
static void read_text_from_stdin(char ** text)
{
    FILE * fp;
    char linestr[4096];
    char * retval = NULL;
    int string_length = 0, i, ctr=0;
    FILE * fp_temp;
    char temp_filename[256];

    fp = stdin;
    if (fp != NULL) {
        sprintf(temp_filename,"%s","/tmp/temp_stylom");
        fp_temp = fopen(temp_filename,"wb");
        if (!fp_temp) {
            printf("read_text_from_stdin: Can't open temporary file\n");
            return;
        }
        /* determine the length of the text needed */
        while (!feof(fp)) {
            retval = fgets(linestr,4095,fp);
            if (retval != NULL) {
                i = fwrite (linestr, strlen(linestr), 1, fp_temp);
                string_length += strlen(linestr);
            }
        }
        fclose(fp_temp);
        /* if nothing was read */
        if (string_length == 0) {
            printf("Warning: nothing loaded\n");
            return;
        }
        /* allocate memory for the text */
        (*text) = (char*)malloc((string_length+1)*sizeof(char));
        if (!(*text)) {
            printf("read_text_from_stdin: Unable to allocate %d bytes\n",string_length+1);
            return;
        }
        /* read the text into the array */
        fp_temp = fopen(temp_filename,"rb");
        if (!fp_temp) {
            printf("read_text_from_stdin: Unable to load %s\n",temp_filename);
            return;
        }
        while (!feof(fp_temp)) {
            retval = fgets(linestr,4095,fp_temp);
            if (retval != NULL) {
                for (i = 0; i < strlen(linestr); i++) {
                    (*text)[ctr++] = linestr[i];
                }
            }
        }
        fclose(fp_temp);
        /* append string terminator */
        (*text)[ctr] = 0;
    }
}

int main(int argc, char* argv[])
{
    int i, show_list=0;
    char * text = 0;
    char name[256],directory[256];
    char image_filename[256], title[256];
    char fingerprint_filename[256];
    stylom_fingerprint fingerprint;
    int function_type = FUNCTION_NONE;
    int stdin_data_exists,memory_allocated=0;

    name[0]=0;
    fingerprint_filename[0]=0;
    sprintf(title,"%s","Text Locations");
    sprintf(image_filename,"%s","result.png");

    /* is there data to be read from stdin? */
    stdin_data_exists = stdin_has_data(1);

    if (stdin_data_exists) {
        memory_allocated=1;
        /* text piped in */
        read_text_from_stdin(&text);
    }
    else {
        /* if no options given then show help */
        if (argc <= 1) {
            show_help();
            return 1;
        }
    }

    /* parse the options */
    for (i = 1; i < argc; i++) {
        /* specify text */
        if ((strcmp(argv[i],"-t")==0) ||
                (strcmp(argv[i],"--text")==0)) {
            i++;
            if (i < argc) {
                /* try assuming the given text is a filename */
                if (read_text_from_file(&text, argv[i])==0) {
                    /* text isn't a filename */
                    text = argv[i];
                    sprintf(text,"%s",argv[i]);
                }
                else {
                    memory_allocated=1;
                }
            }
        }
        /* show fingerprint */
        if ((strcmp(argv[i],"-f")==0) ||
                (strcmp(argv[i],"--fingerprint")==0)) {
            function_type = FUNCTION_FINGERPRINT;
        }
        /* name of the fingerprint */
        if ((strcmp(argv[i],"-n")==0) ||
                (strcmp(argv[i],"--name")==0)) {
            i++;
            if (i < argc) {
                sprintf(name,"%s",argv[i]);
            }
        }
        /* title of graph */
        if (strcmp(argv[i],"--title")==0) {
            i++;
            if (i < argc) {
                sprintf(title,"%s",argv[i]);
            }
        }
        /* return the best match name */
        if ((strcmp(argv[i],"-m")==0) ||
                (strcmp(argv[i],"--match")==0)) {
            i++;
            if (i < argc) {
                function_type = FUNCTION_MATCH;
                show_list = 0;
                sprintf(directory,"%s",argv[i]);
            }
        }
        /* show a list of best matches */
        if ((strcmp(argv[i],"-l")==0) ||
                (strcmp(argv[i],"--list")==0)) {
            i++;
            if (i < argc) {
                function_type = FUNCTION_MATCH;
                show_list = 1;
                sprintf(directory,"%s",argv[i]);
            }
        }
        /* compare the text to the given fingerprint */
        if ((strcmp(argv[i],"-c")==0) ||
                (strcmp(argv[i],"--compare")==0)) {
            i++;
            if (i < argc) {
                function_type = FUNCTION_COMPARE;
                sprintf(fingerprint_filename,"%s",argv[i]);
            }
        }
        /* find words in the current text which are missing
           from the given fingerprint */
        if (strcmp(argv[i],"--missing")==0) {
            i++;
            if (i < argc) {
                function_type = FUNCTION_MISSING;
                sprintf(fingerprint_filename,"%s",argv[i]);
            }
        }
        /* find words in the current text which are more frequently
           used in the given fingerprint */
        if (strcmp(argv[i],"--more")==0) {
            i++;
            if (i < argc) {
                function_type = FUNCTION_MORE;
                sprintf(fingerprint_filename,"%s",argv[i]);
            }
        }
        /* find words in the current text which are less frequently
           used in the given fingerprint */
        if (strcmp(argv[i],"--less")==0) {
            i++;
            if (i < argc) {
                function_type = FUNCTION_LESS;
                sprintf(fingerprint_filename,"%s",argv[i]);
            }
        }
        /* plot fingerprints */
        if ((strcmp(argv[i],"-p")==0) ||
                (strcmp(argv[i],"--plot")==0)) {
            i++;
            if (i < argc) {
                function_type = FUNCTION_PLOT_FINGERPRINTS;
                sprintf(directory,"%s",argv[i]);
            }
        }
        /* plot texts within a directory */
        if (strcmp(argv[i],"--plottexts")==0) {
            i++;
            if (i < argc) {
                function_type = FUNCTION_PLOT_TEXTS;
                sprintf(directory,"%s",argv[i]);
            }
        }
        /* show help */
        if ((strcmp(argv[i],"-h")==0) ||
                (strcmp(argv[i],"--help")==0)) {
            show_help();
            return 1;
        }
        if ((strcmp(argv[i],"-v")==0) ||
                (strcmp(argv[i],"--version")==0)) {
            printf("Version %.2f\n",VERSION);
            return 1;
        }
    }

    /* if some text was given */
    if (text != 0) {
        if (function_type == FUNCTION_FINGERPRINT) {
            /* calculate the fingerprint */
            get_fingerprint(name, text, &fingerprint);
            /* send fingerprint to console */
            write_fingerprint_xml(&fingerprint, stdout);
        }
        if (function_type == FUNCTION_MATCH) {
            /* calculate the fingerprint */
            get_fingerprint(name, text, &fingerprint);
            fingerprint_match(&fingerprint, directory, name, show_list);
            if (show_list == 0) fprintf(stdout,"%s\n",name);
        }
        /* compare the current text to a fingerprint and
           report the differences */
        if (function_type == FUNCTION_COMPARE) {
            fingerprint_compare(text, fingerprint_filename);
        }
        /* report words which are present in the current text
           but which are missing from the given fingerprint */
        if (function_type == FUNCTION_MISSING) {
            fingerprint_missing_words(text, fingerprint_filename);
        }
        /* report words which are present more often in the
           given fingerprint than in the current text */
        if (function_type == FUNCTION_MORE) {
            fingerprint_more_frequent_words(text, fingerprint_filename);
        }
        /* report words which are present less often in the
           given fingerprint than in the current text */
        if (function_type == FUNCTION_LESS) {
            fingerprint_less_frequent_words(text, fingerprint_filename);
        }
    }

    /* plot fingerprints in a diagram */
    if (function_type == FUNCTION_PLOT_FINGERPRINTS) {
        plot_fingerprints(image_filename, title, directory, 640, 640);
    }
    /* plot texts within a directory in a diagram */
    if (function_type == FUNCTION_PLOT_TEXTS) {
        plot_texts(image_filename, title, directory, 640, 640);
    }

    /* free memory */
    if (memory_allocated != 0) {
        if (text != 0) {
            free(text);
        }
    }

    return 1;
}

