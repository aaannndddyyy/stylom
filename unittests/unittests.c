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

#include "../src/stylom.h"

static void test_vocabulary()
{
	stylom_fingerprint fingerprint;
	char * vocab;
	int total_words;

	printf("test_vocabulary...");

	vocab = &fingerprint.vocabulary[0];
	total_words = create_vocabulary("This is a trial test ",
									vocab,
									&fingerprint.vocabulary_frequency[0]);
	assert(vocab != 0);
	if (total_words != 4) {
		printf("\nTotal words %d\n",total_words);
	}
	assert(total_words==4);
	assert(strcmp(&vocab[VINDEX(0,0)],"a")==0);
	assert(strcmp(&vocab[VINDEX((int)('t'-'a'),0)],"test")==0);
	assert(strcmp(&vocab[VINDEX((int)('t'-'a'),1)],"trial")==0);
	assert(strcmp(&vocab[VINDEX((int)('i'-'a'),0)],"is")==0);

	printf("Ok\n");
}

static void test_save_load()
{
	char filename[256],commandstr[256];
	stylom_fingerprint fingerprint1, fingerprint2;
	FILE * fp;
	int result;

	printf("test_save_load...");

	sprintf(filename,"%s","/tmp/temp_test_save_load");

	/* create a fingerprint */
	get_fingerprint("Test", "This is a test.  Some text.", &fingerprint1);

	/* save to file */
	fp = fopen(filename,"w");
	assert(fp);
	write_fingerprint_xml(&fingerprint1, fp);
	fclose(fp);

	/* load from file */
	fp = fopen(filename,"r");
	assert(fp);
	read_fingerprint_xml(&fingerprint2, fp);
	fclose(fp);

	/* compare the two */
	result = fingerprints_identical(&fingerprint1, &fingerprint2, 1);
	if (result != 0) {
		printf("\nResult %d\n",result);
	}
	assert(result==0);

	sprintf(commandstr,"rm %s",filename);
	assert(system(commandstr)==0);

	printf("Ok\n");
}

static void test_similarity()
{
	float similarity;
	stylom_fingerprint fingerprint1, fingerprint2;

	printf("test_similarity...");

	/* create fingerprints */
	get_fingerprint("Test", "This is a test.  Some text.", &fingerprint1);
	get_fingerprint("Test", "This is a test.  Some text.", &fingerprint2);

	similarity = fingerprint_similarity(&fingerprint1,&fingerprint2);
	assert(similarity==100);

	get_fingerprint("Test", "Another test", &fingerprint2);

	similarity = fingerprint_similarity(&fingerprint1,&fingerprint2);
	assert(similarity < 100);
	assert(similarity > 0);

	printf("Ok\n");
}


static void test_average()
{
	char filename[256],directory[256],commandstr[256],name[256];
	stylom_fingerprint fingerprint1, fingerprint_av;
	FILE * fp;
	int i,compare;

	printf("test_average...");

	sprintf(directory,"%s","/tmp/temp_stylom_average");
	sprintf(commandstr,"mkdir %s",directory);
	i = system(commandstr);

	sprintf(name,"%s","Test");

	/* create a nmber of fingerprints */
	for (i = 0; i < 10; i++) {
		sprintf(filename,"%s/test%d",directory,i);

		/* create a fingerprint */
		get_fingerprint(name, "This is a test.  Some text.", &fingerprint1);

		/* save to file */
		fp = fopen(filename,"w");
		assert(fp);
		write_fingerprint_xml(&fingerprint1, fp);
		fclose(fp);
	}

	/* create the average */
	assert(fingerprint_average(&fingerprint_av,directory)!=0);
	sprintf((&fingerprint_av)->name,"%s",name);

	/* compare the average to an instance */
	compare = fingerprints_identical(&fingerprint1,&fingerprint_av,0);
	if (compare != 0) {
		printf("\nCompare %d\n",compare);
	}
	assert(compare==0);

	/* remove the temporary directory */
	sprintf(commandstr,"rm -rf %s",directory);
	i = system(commandstr);

	printf("Ok\n");
}

static void test_create_dictionary()
{
	int total_words,i, max_index=0;
	float dictionary[DICTIONARY_SIZE];

	printf("test_create_dictionary...");

	total_words = create_dictionary("ZZZzzz ZZZzzz ZZZzzz ",dictionary);
	if (total_words != 2) {
		printf("\nTotal words %d\n",total_words);
	}
	assert(total_words == 2);

	
	for (i = 0; i < DICTIONARY_SIZE; i++) {
		if (dictionary[i]>0) max_index = i;
	}
	if (max_index <= DICTIONARY_SIZE*95/100) {
		printf("\nMax index %d%%\n",
			   max_index*100/DICTIONARY_SIZE);
	}
	assert(max_index > DICTIONARY_SIZE*95/100);

	printf("Ok\n");
}

int main(int argc, char* argv[])
{
	test_save_load();
	test_similarity();
	test_average();
	test_create_dictionary();
	test_vocabulary();

	return 1;
}
