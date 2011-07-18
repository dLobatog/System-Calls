#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/*
* split <input binary file> <output binary file1> <output binary file2>
* 
* It splits a binary file into two binary files, each of them with same number of records. 
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

	printf("Writing Record: ");
	recPrint(rec);

	if ((written = write(fh, rec, size))!=sizeof(Record)) {
		perror("write error");
	}
	return written;
}

// Tells whether to print on outputfile1 or outputfile2. When half of the
// file is reached, it starts printing on outputfile2
void recSplitBatch(Record *rec, int size, int fh, int fd) {
	int orsize=size;
	while(size>0) {
		if(size > (orsize/2)) {
			recWriteBin(fh, rec);
			rec++;
			size--;
		}
		if(size <= (orsize/2)) {
			recWriteBin(fd, rec);
			rec++;
			size--;
		}
	}
}

int readBinary(const char *inputfile, const char *outputfile1, const char *outputfile2) {
	char product[52];
	float weight;
	int reference, stock;
	int nlines=0;
	int i=0;
	char line [86];
	int fh;
	int fd;
	// Reads each of the lines in input file and save the records to an array of records.
	if((fh = open(inputfile,O_RDONLY)) < 0) {
		perror("Error opening binary file");
		return 1;
	} else {
		Record rec;
		while(read(fh, &rec, sizeof(Record))> 0) {
			nlines++;
		}
		close(fh);
	}
	if (nlines < 2) {
		printf("File with less than 2 lines. Impossible to split. Exiting\n\n");
		exit(1);
	}
	Record recs[nlines];
	if((fh = open(inputfile,O_RDONLY)) < 0) {
		perror("Error opening binary file");
		return 1;
	} else {
		Record rec;
		while(read(fh, &recs[i], sizeof(Record))>0) {
			i++;
		}
	}
	close(fh);
	// Creates binary files
	if((fh = creat(outputfile1,0666)) < 0) {
		perror("Error when creating binary file");
		return 1;
	}
	if((fd = creat(outputfile2,0666)) < 0) {
		perror("Error when creating binary file");
		return 1;
	}
	recSplitBatch(recs, nlines, fh, fd);
	close(fh);
	close(fd);
	return 0;
}

// read outputfile1 and outputfile2 in order to ensure they were correctly written
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
	if (argc==4) {
		printf("Input file will be %s\nOutput file 1 will be %s\nOutput file 2 will be %s\n\n", argv[1],argv[2],argv[3]);
	} else {
		printf("Wrong number of parameters\n");
		return 1;
	}
	readBinary(argv[1],argv[2],argv[3]);
	printf("\n\n\nThis is a read test for output1\n\n\n");
	testRead(argv[2]);
	printf("\n\n\nThis is a read test for output2\n\n\n");
	testRead(argv[3]);
	return 0;
}

