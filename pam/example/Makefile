CC=gcc
CFLAGS=-Wall
LDFLAGS=-lpam -lpam_misc
TARGET=pam_example
SOURCES=pam_example.c

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)
