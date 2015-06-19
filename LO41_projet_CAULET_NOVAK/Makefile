#Makefile

CXX=gcc
CFLAGS=-g -c
LFLAGS=-g -o 
AUXLIBS=-lm -pthread

TARGET=main.out
SRCFILE=main.c mairie.c utilisateur.c poubelle.c moniteur.c serviceRamassage.c camion.c ipc.c
OBJFILE=$(SRCFILE:.cpp=.o)

# Constantes d'argument
NB_UTILISATEUR=50	#10
NB_PBLN=2			#1
NB_PBLC=3			#1
NB_PBLP=3			#1
NB_PBLV=3			#1
NB_CPBLN=10			#5
NB_CPBLC=2			#1
NB_CPBLP=2			#1
NB_CPBLV=2			#1

#	Compile uniquement l'éxécutable
all : $(TARGET) 

#	Nettoie le répertoire, compile l'éxécutable et le lance
restart: clean $(TARGET) run

$(TARGET): $(OBJFILE)
	@echo "***********************************"
	@echo "> Creating $@"
	@$(CXX) $^ $(LFLAGS) $@ $(LIBS) $(AUXLIBS)
	@echo ""
				
.c.o:
	@reset
	@echo "***********************************"
	@echo "> Generation of $@ from $<"
	@$(CXX) $(CFLAGS) $(INCLUDEDIR) -o $@ $<
	@echo ""

run:
	@echo "***********************************"
	@echo "> Launching $(TARGET)"
	@reset && export LD_LIBRARY_PATH=$(LIBSDIR) && ./$(TARGET) $(NB_UTILISATEUR) $(NB_PBLN) $(NB_PBLC) $(NB_PBLP) $(NB_PBLV) $(NB_CPBLN) $(NB_CPBLC) $(NB_CPBLP) $(NB_CPBLV)

clean: 
	@echo "***********************************"
	@echo "> Cleaning directories"
	@find ./ -type f -name "*.o" -exec rm -rf {} \;
	@find ./ -type f -name "*.swp" -exec rm -rf {} \;
	@rm ./main.out

gdb:
	gdb --args $(TARGET) $(NB_UTILISATEUR) $(NB_PBLN) $(NB_PBLC) $(NB_PBLP) $(NB_PBLV) $(NB_CPBLN) $(NB_CPBLC) $(NB_CPBLP) $(NB_CPBLV)


valgrind :
	valgrind ./$(TARGET) $(NB_UTILISATEUR) $(NB_PBLN) $(NB_PBLC) $(NB_PBLP) $(NB_PBLV) $(NB_CPBLN) $(NB_CPBLC) $(NB_CPBLP) $(NB_CPBLV)

