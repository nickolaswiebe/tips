all:
	gcc tips.c -o tips -std=c99 -Os -s
clean:
	rm tips
install:
	echo 'test'