#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



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

// tells the function recWriteBin to write first all the lines in inputfile1, then all the lines in inputfile2 to the same file
void recMergeBatch(Record *rec1, Record *rec2, int size1, int size2, int fd) {
	while(size1-->0) {
		recWriteBin(fd, rec1);
		rec1++;
	}
	while(size2-->0) {
		recWriteBin(fd, rec2);
		rec2++;
	}

}

int readBinary(const char *inputfile1, const char *inputfile2, const char *outputfile) {
	char product[52];
	float weight;
	int reference, stock;
	int nlines1=0, nlines2=0;
	int i=0;
	char line [86];
	int fh1;
	int fh2;
	int fd;
	// reads lines and records of inputfile1 and saves them to an array of records
	if((fh1 = open(inputfile1,O_RDONLY)) < 0) {
		perror("Error opening binary file");
		return 1;
	} else {
		Record rec;
		while(read(fh1, &rec, sizeof(Record))> 0) {
			nlines1++;
		}
		close(fh1);
	}
	Record recs1[nlines1];
	if((fh1 = open(inputfile1,O_RDONLY)) < 0) {
		perror("Error opening binary file");
		return 1;
	} else {
		Record rec;
		while(read(fh1, &recs1[i], sizeof(Record))>0) {
			i++;
		}
	}
	close(fh1);

	// reads lines and records of inputfile1 and saves them to an array of records
	if((fh2 = open(inputfile2,O_RDONLY)) < 0) {
		perror("Error opening binary file");
		return 1;
	} else {
		Record rec;
		while(read(fh2, &rec, sizeof(Record))> 0) {
			nlines2++;
		}
		close(fh2);
	}
	Record recs2[nlines2];
	i=0;
	if((fh2 = open(inputfile2,O_RDONLY)) < 0) {
		perror("Error opening binary file");
		return 1;
	} else {
		Record rec;
		while(read(fh2, &recs2[i], sizeof(Record))>0) {
			i++;
		}
	}
	close(fh2);
	// create binary file
	if((fd = creat(outputfile,0666)) < 0) {
		perror("Error when creating binary file");
		return 1;
	}
	recMergeBatch(recs1, recs2, nlines1, nlines2, fd);
	close(fd);
	return 0;
}

// reads the output file in order to ensure that everything went ok
int testRead(const char *filename) {
	int fh;

	if((fh = open(filename,O_RDONLY)) < 0) {
		perror("Error opening binary file");
		return 1;
	} else {
		Record rec;
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
		printf("Input file 1 will be %s\nInput file 2 will be %s\nOutput file will be %s\n\n", argv[1],argv[2],argv[3]);
	} else {
		printf("Wrong number of parameters\n");
		return 1;
	}
	readBinary(argv[1],argv[2],argv[3]);
	printf("\n\n\nThis is a read test for output\n\n\n");
	testRead(argv[3]);
	return 0;
}

