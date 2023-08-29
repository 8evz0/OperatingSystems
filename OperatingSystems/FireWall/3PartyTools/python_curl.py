import requests

def download_and_append(url, output_file):
    response = requests.get(url)
    if response.status_code == 200:
        with open(output_file, 'ab') as file:
            file.write(response.content)
            print(f"Содержимое с {url} успешно добавлено к файлу {output_file}")
    else:
        print(f"Произошла ошибка при загрузке содержимого с {url}")

if __name__ == "__main__":
    sites_to_download = [
        "http://lists.blocklist.de/lists/strongips.txt",
        "http://example.org/file2.txt",
        # Добавьте остальные URL сайтов по аналогии
    ]

    output_file = "ipblcklst.txt"
    for site_url in sites_to_download:
        download_and_append(site_url, output_file)

