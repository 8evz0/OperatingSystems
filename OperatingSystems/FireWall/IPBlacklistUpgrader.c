#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

typedef struct {
    FILE *file;
    char error_buffer[CURL_ERROR_SIZE];
} WriteData;

size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    WriteData *data = (WriteData *)userp;
    size_t written = fwrite(contents, size, nmemb, data->file);
    return written;
}

int main(void) {
    CURLM *multi_handle;
    int still_running;

    const char *urls[] = {
        "http://lists.blocklist.de/lists/strongips.txt",
        "https://snort-org-site.s3.amazonaws.com/production/document_files/files/000/025/755/original/ip_filter.blf?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=AKIAU7AK5ITMJQBJPARJ%2F20230808%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Date=20230808T214133Z&X-Amz-Expires=3600&X-Amz-SignedHeaders=host&X-Amz-Signature=5d27469002b7329aaac1edd68ff200beab35a8744e673ddd2ba59aec2067d7b4"
    };
    int num_urls = sizeof(urls) / sizeof(urls[0]);

    curl_global_init(CURL_GLOBAL_DEFAULT);

    multi_handle = curl_multi_init();

    WriteData write_datas[num_urls];

    for (int i = 0; i < num_urls; i++) {
        write_datas[i].file = fopen("blacklist.txt", "a+"); // Измените имя файла
        if (!write_datas[i].file) {
            fprintf(stderr, "Failed to open output file.\n");
            return 1;
        }

        CURL *curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, urls[i]);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &write_datas[i]);
            curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, write_datas[i].error_buffer);
            curl_multi_add_handle(multi_handle, curl);
        }
    }

    do {
        CURLMcode mc = curl_multi_perform(multi_handle, &still_running);
        if (mc != CURLM_OK) {
            fprintf(stderr, "curl_multi_perform() failed: %s\n", curl_multi_strerror(mc));
            break;
        }

        int numfds;
        mc = curl_multi_wait(multi_handle, NULL, 0, 1000, &numfds);

    } while (still_running);

    for (int i = 0; i < num_urls; i++) {
        fclose(write_datas[i].file);
    }

    curl_multi_cleanup(multi_handle);

    curl_global_cleanup();

    return 0;
}
