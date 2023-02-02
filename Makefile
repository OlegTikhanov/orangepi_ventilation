ifneq ($V,1)
Q ?= @
endif

#DEBUG	= -g -O0
DEBUG	= -O3
CC	= gcc
INCLUDE	= -I/usr/local/include
CFLAGS	= $(DEBUG) -Wall $(INCLUDE) -Winline -pipe

LDFLAGS	= -L/usr/local/lib
LDLIBS    = -lwiringPi -lwiringPiDev -lpthread -lm -lcrypt -lrt -lmosquitto

###############################################################################

SRC	=	main.c ventilation.c mqtt.c mhz19b.c

OBJ	=	$(SRC:.c=.o)

BIN	=	ventilation


main:
	$Q echo [link]
	$Q $(CC) $(SRC) -o $(BIN) $(LDFLAGS) $(LDLIBS)

run: 
	$Q sudo ./$(BIN)

install:
	$Q echo [install]
	$Q sudo cp ventilation.service /etc/systemd/system/
	$Q sudo cp $(BIN) /usr/local/bin/
	$Q sudo systemctl daemon-reload
	$Q sudo systemctl enable $(BIN).service
	$Q sudo systemctl start $(BIN).service

uninstall:
	$Q echo [uninstall]
	$Q sudo systemctl stop $(BIN).service
	$Q sudo rm /usr/local/bin/$(BIN)
	$Q sudo rm /etc/systemd/system/ventilation.service
	$Q sudo systemctl daemon-reload

clean:
	$Q echo "[Clean]"
	$Q rm -f $(OBJ) *~ core tags $(BIN)