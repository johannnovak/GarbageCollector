/*	fichier : ipc.h
 *	18/06/2014
 *	Florian CAULET				Johann NOVAK
 *	florian.caulet@utbm.fr	johann.novak@utbm.fr
 */

#include "ipc.h"

void creation_ipc(char* name)
{
	fprintf(stdout, "\nCreation of ipcs");
	ipcID = (int*)malloc(sizeof(int));
	ipcPoubelleCamion = (int*)malloc(sizeof(int));
	ipcSignaux = (int*)malloc(sizeof(int));
	*ipcID = msgget(ftok(name, 'A'), IPC_FLAGS);
	*ipcPoubelleCamion = msgget(ftok(name, 'C'), IPC_FLAGS);
	*ipcSignaux = msgget(ftok(name, 'B'), IPC_FLAGS);
}

void creation_message1(MsgBuffer* m, long type, void* arg1)
{
	m->type = type;
	m->arg[0] = arg1;
}

void creation_message2(MsgBuffer* m, long type, void* arg1, void* arg2)
{
	m->type = type;
	m->arg[0] = arg1;
	m->arg[1] = arg2;
}

void destruction_message(MsgBuffer* m)
{
	fprintf(stdout, "\nDestruction of a message.");
	int i;
	if(m != NULL)
	{
		for(i = 0; i < 2; ++i)
			if(&m->arg[i] != NULL)
			{
				free(&m->arg[i]);
				m->arg[i] = NULL;
			}
		if(m->arg != NULL)
		{
			free(m->arg);
			m->arg = NULL;
		}
		free(m);
		m = NULL;
	}
}

void destruction_ipc()
{
	fprintf(stdout, "\nDestruction of ipcs");
	msgctl(*ipcID,IPC_RMID, NULL);
	msgctl(*ipcPoubelleCamion,IPC_RMID, NULL);
	msgctl(*ipcSignaux,IPC_RMID, NULL);
}
