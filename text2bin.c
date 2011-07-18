#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/*
* text2bin <textfile> <binaryfile>
* 
* It reads a textfile and it prints its content to a binary file given by arguments.
*
*/

// Definition of the structure of each record
typedef struct {
	char product[52];
	float weight;
	int reference, stock;
} Record;

// Prints records
void recPrint(const Record *rec) {
	printf("%s\t%f\t%d\t%d\n", rec->product, rec->weight, rec->reference, rec->stock);
}

// Writes record to a binary file. File descriptor must be passed as argument.
ssize_t recWriteBin(int fh, Record *rec) {
	size_t size = sizeof(Record);
	ssize_t written;

	// Tells the user what record it is going to be written.
	printf("Writing Record: ");
	recPrint(rec);

	if ((written = write(fh, rec, size))!=sizeof(Record)) {
		perror("write error");
	}
	return written;
}

// 'Batch' process. Reads each of the records passed as argument and executes recWriteBin to write them on the binaryfile.
void recWriteBinBatch(int fh, Record *rec, int size) {
	while(size-->0) {
		recWriteBin(fh, rec);
		rec++;
	}
}

int writeBinary(const char *filename, const char *textfilename) {
	int fh;
	// Creates binaryfile
	if((fh = creat(filename,0666)) < 0) {
		perror("Error when creating binary file");
		return 1;
	} else {
		FILE *fp;
		char product[52];
		float weight;
		int reference, stock;
		int nlines=0;
		int i=0;
		char line [86];
		// Opens text file and reads lines
		fp=fopen(textfilename, "r");
		if ( fp != NULL ) {
			while (fgets (line,sizeof line,fp)!= NULL) {
				nlines++;
			}
			printf("%d lines\n",nlines);
		} else {
			perror(filename);
			return(1);
		}
		Record recs[nlines];
		// fopen is executed in order to reposition the pointer to the beginning of the file
		fp=fopen(textfilename, "r");
		// Saves each of the keys of the record in variables
		while (fgets (line,sizeof line,fp)!= NULL) {
			sscanf(line, "%52c%8f%4d%4d", product, &weight, &reference, &stock);
			strcpy(recs[i].product, product);
			recs[i].weight = weight;
			recs[i].reference = reference;
			recs[i].stock = stock;
			i++;
		}
		recWriteBinBatch(fh, recs, nlines);
		close(fp);
		return 0;
	}
}

// read the binaryfile
int testRead(const char *filename) {
	int fh;

	if((fh = open(filename,O_RDONLY)) < 0) {
		perror("Error opening binary file");
		return 1;
	} else {
		Record rec;
		//reads each of the records
		while(read(fh, &rec, sizeof(Record))>0) {
			printf("Read Record: ");
			recPrint(&rec);
		}
		close(fh);
		return 0;
	}
}

int main(int argc, char *argv[]) {
	if (argc==3) {
		printf("Textfile will be %s and binary file will be %s \n", argv[2],argv[1]);
	} else {
		printf("Wrong number of parameters\n");
		return(1);
	}
	writeBinary(argv[2],argv[1]);
	testRead(argv[2]);
	return 0;
}

