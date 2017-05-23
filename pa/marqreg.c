#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <ctype.h>

struct metadata {
	char *outfile;
	char *indexfile;
	char **fieldnames;
	int *fieldtypes;
	int *fieldsizes;
	int fieldcount;
	int totalsize;
};

struct registry {
	char *fields;
};

struct indexer {
	int key;
	int offset;
};


int compare(const void * a, const void * b) {
	struct indexer *orderA = (struct indexer *)a;
	struct indexer *orderB = (struct indexer *)b;
	
	return orderB->key - orderA->key;
}

int getIndex(char *type) {
	int i;

	if (strcmp(type, "int") == 0) {
		return 0;
	}
	if (strcmp(type, "double") == 0) {
		return 1;
	}
	if (strcmp(type, "char") == 0) {
		return 2;
	}
	if (strlen(type) >= 7) {
		char *ref = (char *) malloc((5 + 1) * sizeof(char));
		for (i = 0; i < 5; ++i) {
			ref[i] = type[i];
		}
		ref[5] = 0;
		if (strcmp(ref, "char[") == 0) {
			free(ref);
			return 3;
		}
		free(ref);
	}
	if (strcmp(type, "float") == 0) {
		return 4;
	}
	return 0;
}

int getSize(char *token) {
	int ret, p = 0;
	char *buffer;

	if (strcmp(token, "int") == 0) {
		return 4;
	}
	if (strcmp(token, "double") == 0) {
		return 8;
	}
	if (strcmp(token, "char") == 0) {
		return 1;
	}
	if (strcmp(token, "float") == 0) {
		return 4;
	}
	buffer = (char *) malloc(80 * sizeof(char));
	while (token[p + 5] >= '0' && token[p + 5] <= '9') {
		buffer[p] = token[p + 5];
		++p;
	}
	ret = atoi(buffer);
	free(buffer);
	return ret;
}

int search(struct indexer *ind, int target, int nregs) {
	int lo = 0, hi = nregs - 1;
	while (lo <= hi) {
		int mid = (lo + hi) / 2;
		int ret = ind[mid].key;
		if (ret == target) {
			return ind[mid].offset;
		}
		if (ret < target) {
			lo = mid + 1;
		} else {
			hi = mid - 1;
		}
	}
	return -1;
}

char *trimwhitespace(char *str) {
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}

char *trimquote(char *str) {
  char *end;

  // Trim leading quote
  while(*str == '"') str++;

  if(*str == 0)  // All quotes?
    return str;

  // Trim trailing quote
  end = str + strlen(str) - 1;
  while(end > str && *end == '"') end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}

int countMetadata(FILE *fp) {
	int count = 0;
	char ch;
	char *line, *token;

	line = (char *) malloc(80 * sizeof(char));
	while ((ch = getc(fp)) != EOF) {
		line[0] = ch;
		int p = 1;
		while ((ch = getc(fp)) != '\n' && ch != EOF) {
			line[p++] = ch; 
		}
		line[p] = 0;
		token = strtok(line, ": ");
		while (token != NULL) {

			if (strcmp(token, "field-name") == 0 || strcmp(token, "key-name") == 0) {
				++count;
			}
			token = strtok(NULL, ": ");
		}
	}
	fseek(fp , 0, SEEK_SET);
	free(line);

	return count;
}

void readLine(FILE *fp, char **str) {
	int p;
	char ch;
	char *line, *name; 

	line = (char *) malloc(80 * sizeof(char));

	p = 0;
	while ((ch = getc(fp)) != '\n' && ch != EOF) {
		line[p++] = ch; 
	}
	line[p] = 0;
	strtok(line, ": ");
	name = strdup(strtok(NULL, ": "));

	*str = malloc(80 * sizeof(char));
	strcpy(*str, name);

	free(line);

}

void readBlock(FILE *fp, struct metadata *md, int pos) {
	int p, idx, size = -1;
	char ch;
	char *line, *name, *type; 

	line = (char *) malloc(80 * sizeof(char));

	p = 0;
	while ((ch = getc(fp)) != '\n' && ch != EOF) {
		line[p++] = ch; 
	}
	line[p] = 0;
	strtok(line , ": ");
	name = strdup(strtok(NULL, ": "));
	idx = getIndex("char[80]");
	strcpy(md->fieldnames[pos], name);


	p = 0;
	while ((ch = getc(fp)) != '\n' && ch != EOF) {
		line[p++] = ch; 
	}
	line[p] = 0;
	strtok(line, ": ");
	type = strdup(strtok(NULL, ": "));
	
	idx = getIndex(type);
	
	size = getSize(type);
	md->fieldsizes[pos] = size;

	md->fieldtypes[pos] = idx;
	
	free(line);
	
}

void readMetadata(FILE *fp, struct metadata *md) {
	int i, count;

	md->outfile = malloc(80 * sizeof(char));
	md->indexfile = malloc(80 * sizeof(char));

	count = countMetadata(fp);

	readLine(fp, &md->outfile);
	strcpy(md->indexfile, md->outfile);
	i = strlen(md->outfile) - 4;
	md->indexfile[i++] = '.';
	md->indexfile[i++] = 'i';
	md->indexfile[i++] = 'd';
	md->indexfile[i++] = 'x';

	md->fieldnames = malloc(count * sizeof(char *));
	for (i = 0; i < count; ++i) {
		md->fieldnames[i] = (char *) malloc(80 * sizeof(char));
	}
	md->fieldtypes = malloc(count * sizeof(int));
	md->fieldsizes = malloc(count * sizeof(int));
	md->fieldcount = count;

	readBlock(fp, md, 0);

	for (i = 1; i < count; ++i) {
		readBlock(fp, md, i);
	}

	/*
	printf("Midway results\n");
	printf("%s\n", md->fieldnames[0]);
	printf("%d\n", md->fieldtypes[0]);
	for (i = 1; i < count; ++i) {
		printf("%s\n", md->fieldnames[i]);
		printf("%d\n", md->fieldtypes[i]);
	}
*/
}

char *strdup(const char *s) {
	char *d = malloc(strlen(s) + 1);
	if (d == NULL) {
		return NULL;
	}
	strcpy(d,s);
	return d;
}

int calcSize(struct metadata *md) {
	int i, acc = 0;

	for (i = 0; i < md->fieldcount; ++i) {
		acc += md->fieldsizes[i];
	}
	return acc;
}

int searchAlternate(FILE *fp, struct metadata *md, int target) {
	fseek(fp, 0, SEEK_SET);
	int key, offset = -md->totalsize; 
	do {
		fread(&key, sizeof(int), 1, fp);
		fseek(fp, md->totalsize - sizeof(int), SEEK_CUR);
		offset += md->totalsize;
//	printf("That's it %d %d %d %d\n", target, key, md->totalsize, offset);
	} while (target != key);
//	printf("result %d %d\n", target, key);
	return offset;
}

int executeCommand(FILE *fp, FILE *fp2, char *line, struct metadata *md) {
	int i, j, x;
	double y;
	float z;
	char *data;
	
//	int npos = fseek(fp, 0, SEEK_END);
//	int nregs = npos / md->totalsize;
//	fclose(fp);

	char *token = strdup(strtok(line, " "));
//		printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! %s\n", line);
	
	if (strcmp(token, "insert") == 0) {
		for (j = 0; j < md->fieldcount; ++j) {
			token = strdup(strtok(NULL, ","));
			token = trimwhitespace(token);
			token = trimquote(token);
			switch (md->fieldtypes[j]) {
				case 0:
					x = atoi(token);
					fwrite(&x, sizeof(char), md->fieldsizes[j], fp2);
					break;
				case 1:
					y = atof(token);
					fwrite(&y, sizeof(char), md->fieldsizes[j], fp2);
					break;
				case 2:
					fwrite(&token[0], sizeof(char), md->fieldsizes[j], fp2);
					break;
				case 3:
					fwrite(token, sizeof(char), md->fieldsizes[j], fp2);
					break;
				case 4:
					z = (float) atof(token);
					fwrite(&z, sizeof(char), md->fieldsizes[j], fp2);
			}
			++i;
//			printf(">>>>>> )))))) %d\n", ftell(fp2));
		}
		fflush(fp2);
	}
	if (strcmp(token, "index") == 0) {
		/*
		printf("ok 2\n");
		list = (struct indexer *) malloc(nregs * sizeof(struct indexer));
		i = 0;
		fp = fopen(md->indexfile, "rb");
		fseek(fp, 0, SEEK_SET);
		while (fseek(fp, md->totalsize, SEEK_CUR)) {
			fread(&list[i].key, sizeof(int), 1, fp);
			list[i].offset = i * md->totalsize;
			++i;
		}
		fclose(fp);
		qsort(list, nregs, sizeof(struct indexer), compare);
		
		fp2 = fopen(md->indexfile, "wb");

		for (i = 0; i < nregs; ++i) {
			fwrite(&list[i].key, sizeof(int), 1, fp2);
			fwrite(&list[i].offset, sizeof(int), 1, fp2);
		}
		fclose(fp2);
		printf("ok 22\n");
		*/
	}
	if (strcmp(token, "search") == 0) {
		token = strtok(NULL, " ");
		int target = atoi(token);

		int pos = searchAlternate(fp, md, target);
		fseek(fp, pos, SEEK_SET);
			
		data = malloc(md->totalsize);
		for (i = 0; i < md->fieldcount; ++i) {
			fread(data, sizeof(char), md->fieldsizes[i], fp);

			switch (md->fieldtypes[i]) {
				case 0:
					printf("%d\n", *((int *) data));
					break;
				case 1:
					printf("%.2f\n", *((double *) data));
					break;
				case 2:
					printf("%c\n", *((char *) data));
					break;
				case 3:
					printf("%s\n", (char *) data);
					break;
				case 4:
					printf("%.2f\n", *((float *) data));
			}
		}
		free(data);
	}
	token[4] = 0;
	if (strcmp(token, "exit") == 0) {
		return 1;
	}
	return 0;
}

int main() {
	char *input, *line;
	FILE *fp, *fp2, *fp3;
	struct metadata md;

	input = (char *) malloc(80 * sizeof(char));
	scanf("%s\n", input);
	fp = fopen(input, "rb");


	readMetadata(fp, &md);

	md.totalsize = calcSize(&md);

//	printf("Final results\n");
//	printf("%s\n", md.fieldnames[0]);
//	printf("%d\n", md.fieldtypes[0]);
	
//	for (i = 1; i < nfields; ++i) {
//		printf("%s\n", md.fieldnames[i]);
//		printf("%d\n", md.fieldtypes[i]);
//	}

	fp2 = fopen(md.outfile, "rb");
	fp3 = fopen(md.outfile, "wb");
	line = malloc(80 * sizeof(char));
	while (1) {
		size_t len = 0;
		getline(&line, &len, stdin);
		if (executeCommand(fp2, fp3, line, &md)) {
			break;
		}
//	exit(0);
	}
	free(md.outfile);
	free(md.indexfile);

	fclose(fp);
	fclose(fp2);
	fclose(fp3);
	free(line);

	return 0;
}
