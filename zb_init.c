/*
Copyright (C) 2000 Shane Powell

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

//
// q2admin
//
// zb_init.c
//
// copyright 2000 Shane Powell
//

#include "g_local.h"

game_import_t  gi;
game_export_t  globals;
game_export_t  *dllglobals;

cvar_t  *rcon_password, *gamedir, *maxclients, *logfile, *rconpassword, *port, *serverbindip;	// UPDATE

qboolean quake2dirsupport = TRUE;

char dllname[256];
char gmapname[MAX_QPATH];

qboolean dllloaded = FALSE;

char moddir[256];

int randomwaitreporttime = 55;

int maxMsgLevel = 3;

char *zbotversion = "Q2Admin Version " Q2ADMINVERSION "\n";
qboolean serverinfoenable = TRUE;

char buffer[0x10000];
char buffer2[256];

int logfilecheckcount;

int lframenum;
float  ltime;

char  com_token[MAX_TOKEN_CHARS];

/*
==============
COM_Parse
 
Parse a token out of a string
==============
*/
char *COM_Parse(char **data_p, char **command_p)
{
	int c;
	int len;
	char *data;
	
	data = *data_p;
	len= 0;
	com_token[0]= 0;
	
	if(!data)
		{
			*data_p= NULL;
			return"";
		}
		
	// skip whitespace
skipwhite:
	while ( (c = *data) <= ' ')
		{
			if (c == 0)
				{
					*data_p = NULL;
					return "";
				}
			data++;
		}
		
	// skip // comments
	if (c=='/' && data[1] == '/')
		{
			while (*data && *data != '\n')
				data++;
			goto skipwhite;
		}
		
	// handle quoted strings specially
	if (c == '\"')
		{
			data++;
			
			if(command_p)
				{
					*command_p = data;
				}
				
			while (1)
				{
					c = *data++;
					if (c=='\"' || !c)
						{
							com_token[len] = 0;
							*data_p = data;
							return com_token;
						}
					if (len < MAX_TOKEN_CHARS)
						{
							com_token[len] = c;
							len++;
						}
				}
		}
		
	if(command_p)
		{
			*command_p = data;
		}
		
	// parse a regular word
	do
		{
			if (len < MAX_TOKEN_CHARS)
				{
					com_token[len] = c;
					len++;
				}
			data++;
			c = *data;
		}
	while (c>32);
	
	if (len == MAX_TOKEN_CHARS)
		{
			//    Com_Printf ("Token exceeded %i chars, discarded.\n", MAX_TOKEN_CHARS);
			len = 0;
		}
	com_token[len] = 0;
	
	*data_p = data;
	return com_token;
}

void InitGame (void)
{
	gi.dprintf (zbotversion);
	
	if(!dllloaded) return;
	
	// be careful with all functions called from this one (like dprintf_internal) to not use
	// proxyinfo pointer because it's not initialized yet. -Harven
	dllglobals->Init();
	
	copyDllInfo();
	
	maxclients = gi.cvar ("maxclients", "4", 0);
}


void SpawnEntities (char *mapname, char *entities, char *spawnpoint)
{
	char *backupentities = entities;
	
	if(!dllloaded) return;
	
	dllglobals->SpawnEntities(mapname, backupentities, spawnpoint);
	copyDllInfo();
	return;
}

qboolean ClientConnect (edict_t *ent, char *userinfo)
{
	qboolean ret;

	if(!dllloaded) return FALSE;
	
	ret = dllglobals->ClientConnect(ent, userinfo);
	copyDllInfo();
	return ret;
}


void ClientUserinfoChanged (edict_t *ent, char *userinfo)
{
	if(!dllloaded) return;
	
	dllglobals->ClientUserinfoChanged(ent, userinfo);
	copyDllInfo();
	return;
}

void ClientDisconnect (edict_t *ent)
{
	if(!dllloaded) return;
	
	dllglobals->ClientDisconnect(ent);
	copyDllInfo();
	return;
}


void ClientBegin (edict_t *ent)
{
	if(!dllloaded) return;
	
	dllglobals->ClientBegin(ent);
	copyDllInfo();
	return;
}

void WriteGame (char *filename, qboolean autosave)
{
	if(!dllloaded) return;
	
	dllglobals->WriteGame(filename, autosave);
	copyDllInfo();
	return;
}


void ReadGame (char *filename)
{
	if(!dllloaded) return;
	
	dllglobals->ReadGame(filename);
	copyDllInfo();
	return;
}

void WriteLevel (char *filename)
{
	if(!dllloaded) return;
	
	dllglobals->WriteLevel(filename);
	copyDllInfo();
	return;
	
}

void ReadLevel (char *filename)
{
	if(!dllloaded) return;
	
	dllglobals->ReadLevel(filename);
	copyDllInfo();
	return;
	
}

