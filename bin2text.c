#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/*
* bin2text <binaryfile> <textfile>
* 
* It reads a binary file and it prints its content to a text file given by arguments.
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

// Writes record to a text file. File pointer must be passed as argument.
ssize_t recWriteText(FILE *file, Record *rec) {
	ssize_t written;
	
	printf("Writing Record: "); recPrint(rec);

	if ((written = fprintf(file,"%52s%8g\t%4d\t%4d\n", rec->product, rec->weight, rec->reference, rec->stock))<0) {
		perror("write error");
	}
	return written;
}

// 'Batch' process. Reads each of the records passed as argument and executes recWriteText to write them on the text file
void recWriteTextBatch(FILE *file, Record *rec,int size) {
	while(size-->0) {
		recWriteText(file, rec);
		rec++;
	}
}

int readBinary(const char *filename, const char *textfilename) {
	FILE *file; 
	// Creates binaryfile
	if ((file = fopen(textfilename,"a+")) == NULL) {
		perror("Error when creating text file");
		return 1;	
	} else {
		char product[52];
		float weight;
		int reference, stock;
		int nlines=0;
		int i=0;
	        char line [86];
		// Opens binary file and reads lines
		int fh;
		if((fh = open(filename,O_RDONLY)) < 0){
	 		perror("Error opening binary file");
			return 1;
		} else {
			Record rec;
			while(read(fh, &rec, sizeof(Record))> 0) {
				nlines++;
		}
		close(fh);
	        printf("%d lines\n",nlines);
		}
		Record recs[nlines];
		// open is executed in order to reposition the descriptor to the beginning of the file
	        if((fh = open(filename,O_RDONLY)) < 0){
	 		perror("Error opening binary file");
			return 1;
		} else {
			// Saves each of the records in a structure
			Record rec;
			while(read(fh, &recs[i], sizeof(Record))>0) {
 				i++;				
			}
		}
		recWriteTextBatch(file, recs, nlines);
		fclose(file);
		close(fh);
		return 0;
	}
}

// read the text file
int testReadText(const char *textfilename) {
	FILE *file; 
	if ((file = fopen(textfilename,"r")) == NULL) {
		perror("Error when reading text file");
		return 1;	
	}
	else {
		Record rec;
		char line [86];
		char product[52];
		float weight;
		int reference, stock;		
		while (fgets (line,sizeof line,file)!= NULL){
			printf("Read Record: ");
		      	sscanf(line, "%52c%8f%4d%4d", product, &weight, &reference, &stock);
			printf("%52s%8g%4d%4d\n", product, weight, reference, stock);
		}
		fclose(file);
		return 0;
	}
}

int main(int argc, char *argv[]){
	if (argc==3){
		printf("Binaryfile will be %s and text file will be %s \n", argv[1],argv[2]);
	}
	else{
		printf("Wrong number of parameters\n");
		return 1;
	}
	readBinary(argv[1],argv[2]);
	testReadText(argv[2]);
	return 0;
}
