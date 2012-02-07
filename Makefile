LABDEFS =

C++ = g++
CC = g++
AR = ar
ARFLAGS = ru
RANLIB = ranlib


COMMONSRCS = BufSchedOutputStream.cc ScheduledInputStream.cc InputStream.cc ScheduledOutputStream.cc OutputStream.cc sthread.cc Stats.cc util.cc NWScheduler.cc Flag.cc AlarmThread.h AlarmThread.cc STFQNWScheduler.cc MaxNWScheduler.cc

COMMONOBJS := $(COMMONSRCS:.cc=.o) SocketLibrary/libsock.a

OTHERSRCS = sender.cc receiver.cc unit.cc common.c sendAndRecv.cc

SRCS = $(COMMONSRCS) $(OTHERSRCS)

LIBS = -L./SocketLibrary/ -lpthread -lrt -lsock

CFLAGS = -g -Wall -D_POSIX_THREAD_SEMANTICS -Werror $(LABDEFS)

all: SocketLibrary/libsock.a sender receiver unit sendAndRecv


%.o: %.cc
	$(C++) $(CFLAGS) $< -c -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $@

#
# Etags is really useful for navigating source code 
# from emacs. Try it out.
#
etags:
	etags *.cc *.c SocketLibrary/*.c

SocketLibrary/libsock.a:
	cd SocketLibrary && make

unit: unit.o common.o $(COMMONOBJS)
	$(CC) -o unit unit.o common.o $(COMMONOBJS) $(LIBS) 

sendAndRecv: sendAndRecv.o common.o $(COMMONOBJS)
	$(CC) -o sendAndRecv sendAndRecv.o common.o $(COMMONOBJS) $(LIBS) 

sender: sender.o common.o $(COMMONOBJS)
	$(CC) -o sender sender.o common.o $(COMMONOBJS) $(LIBS) 

receiver: receiver.o common.o $(COMMONOBJS)
	$(CC) -o receiver receiver.o common.o $(COMMONOBJS) $(LIBS) 

test: test.o
	$(CC) -o test test.o common.o $(COMMONOBJS) $(LIBS)

simpleGraph: sender receiver example.gnuplot
	receiver 5000 & > /dev/null; sleep 1; sender 127.0.0.1 5000 2 | gawk -f diff.awk > tmp.dat; gnuplot example.gnuplot; echo "Done. Graph is in tmp.ps. Note that the receiver is still running. You may want to kill it."


plot1.ps: sendAndRecv plot1.gnuplot
		sendAndRecv  > data1; gnuplot plot1.gnuplot; echo "Done. Graph is in plot1.ps."

plot2.ps: sendAndRecv plot2.gnuplot
		sendAndRecv 100000000 1000000 > data2; gnuplot plot2.gnuplot; echo "Done. Graph is in plot2.ps."

plot3.ps: sendAndRecv plot3.gnuplot
		sendAndRecv 100000000 1000000 stfq > data3; gnuplot plot3.gnuplot; echo "Done. Graph is in plot3.ps."

plot1b.ps: sendAndRecv plot1.gnuplot
		sendAndRecv 2000000000 > data1b; gnuplot plot1b.gnuplot; echo "Done. Graph is in plot1b.ps."

plot2b.ps: sendAndRecv plot2b.gnuplot
		sendAndRecv 5000000 1000000 > data2b; gnuplot plot2b.gnuplot; echo "Done. Graph is in plot2b.ps."

plot3b.ps: sendAndRecv plot3b.gnuplot
		sendAndRecv 5000000 1000000 stfq > data3b; gnuplot plot3b.gnuplot; echo "Done. Graph is in plot3b.ps."

plots: plot1.pdf plot2.pdf plot3.pdf plot1b.pdf plot2b.pdf plot3b.pdf


%.pdf: %.ps
	ps2pdf $< 

clean:
	/bin/rm -f sender receiver unit *.o core *~ TAGS tmp.dat tmp.ps *.log *.aux
	cd SocketLibrary && make clean

realclean: clean
	/bin/rm -f *.pdf *ps data1 data2 data3 data1b data2b data3b


#
# You should change this rule if you use a program
# other than latex for your report.
#
report.pdf: report.tex plot1.pdf plot2.pdf plot3.pdf plot1b.pdf plot2b.pdf plot3b.pdf
	pdflatex report.tex

report: report.pdf


TURNIN := /lusr/bin/turnin
GRADER := impjdi
LAB_NAME := handin-372-labT
handin: handin.tar
	echo "Turning in handin.tar containing the following files:"
	tar tf handin.tar
	$(TURNIN) --submit $(GRADER) $(LAB_NAME) handin.tar


handin.tar: clean
	tar cf handin.tar `find . -type f | grep -v '^\.*$$' | grep -v '/CVS/' | grep -v '/\.svn/' | grep -v '/\.git/' | grep -v 'lab[0-9].*\.tar\.gz' | grep -v '/\~/' | grep -v 'labThread.html' | grep -v '/\.tar/'` 



