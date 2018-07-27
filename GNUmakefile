#	$Id: GNUmakefile,v 1.4 2012/01/06 10:13:55 culot Exp $

VERSION=        20120131

PRJ=		$(shell basename `pwd`)
EXE=		$(PRJ)
BAK=		$(PRJ)-$(shell date '+%Y%m%d-%H%M').tar.gz
PKG=		$(PRJ)-$(VERSION).tar.gz
SRCDIR=		./

CC?=		gcc
CFLAGS+=	-Wall -DVERSION="\"$(VERSION)\""
LDFLAGS+=
LIBS+=

#FDEBUG= 1

ifdef FDEBUG
CFLAGS+= -g -ggdb -DDEBUG
endif

SRCS=		$(wildcard $(SRCDIR)/*.c)
-include config.mk
OBJ=		$(SRCS:.c=.o)


all:	$(EXE)

$(EXE):	$(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o:	%.c
	$(CC) $(CFLAGS) -o $@ -c $<

backup:	clean-all
	@cd .. && tar czvf $(BAK) $(PRJ) >/dev/null && mv $(BAK) $(PRJ)
	@echo "==> Backup created: $(BAK)"

dist:	clean-all
	@cd .. && tar -czvf $(PKG) --exclude='CVS' $(PRJ) >/dev/null && mv $(PKG) $(PRJ)
	@echo "==> Package created: $(PKG)"

clean-all: clean
	@rm -f config.mk config.h TAGS

clean:
	@rm -f $(OBJ) $(EXE)
	@rm -f core
	@rm -f *.gz
	@echo "==> Project cleaned"

tags:
	@etags *.[ch]
	@echo "==> Tags generated"

count:
	@wc -l *.[ch]

list-todo:
	@grep -n XXX *.[ch]

list-targets:
	@cat GNUmakefile | grep -E ^[^[:space:]\.$$%]+: | cut -d: -f1

.PHONY:	all backup dist clean clean-all tags count list-todo list-targets
