#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <curl/curl.h>
#define CH_NUM 4;
#define MiB 1048576;
using namespace std;

struct MemoryStruct
{
	char* memory;
	size_t size;
};

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)userp;

	mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
	if (mem->memory == NULL) {
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	memcpy(&(mem->memory[mem->size]), ptr, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}

int main(int argc, const char* argv[])
{
	if (argc < 2)
	{
		cout << "Usage: multiGet.exe [Options] url\n" << "\t-o string\n\t\tWrite output to <file> instead of default\n";
			//TODO << "\t-parallel\n\t\tDownload chunks in parallel instead of sequentially\n";
		exit(1);
	}
	FILE* outputFile;

	int i = 1; //command line arguments iterator

	if (strcmp(argv[i], "-o") == 0)
	{
		//TODO: add test case when filename is missing
		outputFile = fopen(argv[++i],"wb");
		i++;
	}
	else 
		outputFile = fopen("Download4MiB.jar","wb"); //default file

	if (strcmp(argv[i], "-parallel") == 0)
	{
		//TODO
		cout << "Not available in demo version\n";
		i++;
	}
	if (i >= argc) //checks if an url was provided
	{
		cout << "Usage: multiGet.exe [Options] url\n" << "\t-o string\n\t\tWrite output to <file> instead of default\n";
		//<< "\t-parallel\n\t\tDownload chunks in parallel instead of sequentially\n";
		exit(1);
	}
	string url = argv[i];


	size_t chunkNumber = CH_NUM;
	MemoryStruct* chunks = new MemoryStruct[chunkNumber];
	for (int j = 0; j < chunkNumber; j++) { chunks[j].memory = new char[1]; chunks[j].size = 0; }

	CURL *curl;
	CURLcode res;
	curl = curl_easy_init();

	int lowerbound = 0;
	int upperbound = MiB;
	for (int j = 0; j < chunkNumber; j++)
	{

		if (curl) {
			curl_easy_setopt(curl, CURLOPT_URL, url);
			std::stringstream range;
			range << lowerbound << "-" << upperbound-1; //specify bytes range
			curl_easy_setopt(curl, CURLOPT_RANGE, (range.str()).c_str()); 
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)(chunks+j)); //write to the #j chunk
			res = curl_easy_perform(curl); //GET
		}
		lowerbound = upperbound;
		upperbound += MiB;
	}
	curl_easy_cleanup(curl);

	for (int j = 0; j < chunkNumber; j++) //write to file
	{
		fwrite(chunks[j].memory, sizeof(char), chunks[j].size, outputFile);
	}
	fclose(outputFile);
	cout << "Happy end!\n";
	return 0;
}