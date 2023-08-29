# Поменять названия

.PHONY: all clean

all: entry IPBlacklistUpgrader M0duleF0rNetF1lter 

IPBlacklistUpgrader:
	gcc -o IPBlacklistUpgrader IPBlacklistUpgrader.c -lcurl -uthash

M0duleF0rNetF1lter:
	gcc -g -o M0duleF0rNetf1lter M0duleF0rNetf1lter.c -lnet -lnetfilter_queue -lcurl -lxml2
	
entry: entry.c
	gcc -g $< -o $@ -ldl -lpam -lcrypt -lpam_misc -lcurl

clean:
	rm -f entry
	rm -f IPChanger
	find . -iname 'M0duleF0rNetf1lter' -delete
	find . -iname 'IPBlacklistUpgrader' -delete
